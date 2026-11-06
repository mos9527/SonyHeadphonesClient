import { socket } from '@kit.ConnectivityKit';
import { BusinessError } from '@kit.BasicServicesKit';

export enum SessionState {
  DISCONNECTED,
  CONNECTING,
  CONNECTED
}

export function errorMessage(error: BusinessError): string {
  return `${error.code}: ${error.message}`;
}

export function formatHex(buffer: ArrayBuffer): string {
  const bytes = new Uint8Array(buffer);
  const limit = Math.min(bytes.length, 256);
  let result = '';
  for (let index = 0; index < limit; ++index) {
    if (index > 0) {
      result += ' ';
    }
    result += bytes[index].toString(16).padStart(2, '0');
  }
  return bytes.length > limit ? `${result} ... (${bytes.length} bytes)` : result;
}

export function parseHex(text: string): ArrayBuffer {
  const hex = text.replace(/\s/g, '');
  if (hex.length === 0 || hex.length % 2 !== 0 || !/^[0-9a-fA-F]+$/.test(hex)) {
    throw new Error('Enter complete hexadecimal bytes, e.g. 3E 01 00.');
  }
  if (hex.length > 8192) {
    throw new Error('A single write cannot exceed 4096 bytes.');
  }
  const buffer = new ArrayBuffer(hex.length / 2);
  const bytes = new Uint8Array(buffer);
  for (let index = 0; index < bytes.length; ++index) {
    bytes[index] = parseInt(hex.substring(index * 2, index * 2 + 2), 16);
  }
  return buffer;
}

export class RfcommSession {
  private clientSocket: number = -1;
  private generation: number = 0;
  private writePending: boolean = false;
  private state: SessionState = SessionState.DISCONNECTED;
  private onState: (state: SessionState) => void;
  private onLog: (message: string) => void;
  private onReceive: (data: ArrayBuffer) => void;

  constructor(onState: (state: SessionState) => void, onLog: (message: string) => void,
    onReceive: (data: ArrayBuffer) => void) {
    this.onState = onState;
    this.onLog = onLog;
    this.onReceive = onReceive;
  }

  private setState(state: SessionState): void {
    this.state = state;
    this.onState(state);
  }

  private closeSocket(clientSocket: number): void {
    try {
      socket.sppCloseClientSocket(clientSocket);
    } catch (error) {
      this.onLog(`Failed to close socket: ${errorMessage(error as BusinessError)}`);
    }
  }

  disconnect(): void {
    ++this.generation;
    const previousSocket = this.clientSocket;
    this.clientSocket = -1;
    this.writePending = false;
    this.setState(SessionState.DISCONNECTED);
    if (previousSocket >= 0) {
      this.closeSocket(previousSocket);
    }
  }

  connect(deviceId: string, uuid: string, secure: boolean): void {
    if (this.state !== SessionState.DISCONNECTED) {
      throw new Error('Disconnect the current session first.');
    }
    const currentGeneration = ++this.generation;
    const options: socket.SppOptions = {
      uuid: uuid,
      secure: secure,
      type: socket.SppType.SPP_RFCOMM
    };
    this.setState(SessionState.CONNECTING);
    this.onLog(`Connecting to ${deviceId}, UUID=${uuid}, secure=${secure}`);
    try {
      socket.sppConnect(deviceId, options, (error: BusinessError, clientSocket: number) => {
        if (currentGeneration !== this.generation) {
          if (!error && clientSocket >= 0) {
            this.closeSocket(clientSocket);
          }
          return;
        }
        if (error) {
          this.onLog(`Connection failed: ${errorMessage(error)}`);
          this.disconnect();
          return;
        }
        this.clientSocket = clientSocket;
        this.onLog(`RFCOMM connected, socket=${clientSocket}`);
        this.setState(SessionState.CONNECTED);
        this.readLoop(clientSocket, currentGeneration);
      });
    } catch (error) {
      this.onLog(`Connection failed: ${errorMessage(error as BusinessError)}`);
      this.disconnect();
    }
  }

  private async readLoop(clientSocket: number, currentGeneration: number): Promise<void> {
    try {
      while (currentGeneration === this.generation && this.clientSocket === clientSocket) {
        const buffer = await socket.sppReadAsync(clientSocket);
        if (currentGeneration !== this.generation) {
          return;
        }
        if (buffer.byteLength === 0) {
          this.onLog('The remote device returned no data. Closing the session.');
          this.disconnect();
          return;
        }
        this.onLog(`RX ${formatHex(buffer)}`);
        this.onReceive(buffer);
      }
    } catch (error) {
      if (currentGeneration === this.generation) {
        this.onLog(`Read failed: ${errorMessage(error as BusinessError)}`);
        this.disconnect();
      }
    }
  }

  async write(buffer: ArrayBuffer): Promise<void> {
    if (this.state !== SessionState.CONNECTED || this.clientSocket < 0) {
      throw new Error('RFCOMM is not connected.');
    }
    if (this.writePending) {
      throw new Error('The previous write is still pending.');
    }
    if (buffer.byteLength === 0 || buffer.byteLength > 4096) {
      throw new Error('A single write must contain 1 to 4096 bytes.');
    }
    const currentGeneration = this.generation;
    const clientSocket = this.clientSocket;
    this.writePending = true;
    try {
      await socket.sppWriteAsync(clientSocket, buffer);
      if (currentGeneration === this.generation) {
        this.onLog(`TX ${formatHex(buffer)}`);
      }
    } catch (error) {
      if (currentGeneration === this.generation) {
        this.onLog(`Write failed: ${errorMessage(error as BusinessError)}`);
        this.disconnect();
        throw new Error(errorMessage(error as BusinessError));
      }
    } finally {
      if (currentGeneration === this.generation) {
        this.writePending = false;
      }
    }
  }
}
