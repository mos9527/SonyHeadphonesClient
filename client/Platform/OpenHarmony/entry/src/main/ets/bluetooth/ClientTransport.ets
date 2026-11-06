import { abilityAccessCtrl, common } from '@kit.AbilityKit';
import { access, connection } from '@kit.ConnectivityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import nativeClient, { ClientDevice, TransportSnapshot } from 'libmain.so';
import { RfcommSession, SessionState, errorMessage } from './RfcommSession';

export class ClientTransport {
  private active: boolean = false;
  private authorized: boolean = false;
  private revision: number = 0;
  private timer: number = -1;
  private session?: RfcommSession;
  private generation: number = 0;
  private writing: boolean = false;
  private failure: string = 'Grant Bluetooth permission and turn on Bluetooth.';
  private onStatus: (message: string) => void;

  constructor(onStatus: (message: string) => void) {
    this.onStatus = onStatus;
  }

  async start(context: common.UIAbilityContext): Promise<void> {
    if (this.active) {
      return;
    }
    this.active = true;
    const revision = ++this.revision;
    this.authorized = false;
    try {
      const result = await abilityAccessCtrl.createAtManager().requestPermissionsFromUser(
        context, ['ohos.permission.ACCESS_BLUETOOTH']);
      if (!this.active || revision !== this.revision) {
        return;
      }
      this.authorized = result.authResults.length === 1 &&
        result.authResults[0] === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
      this.failure = this.authorized ? 'Turn on Bluetooth and pair your headphones in system settings.' : 'Bluetooth permission not granted.';
    } catch (error) {
      if (!this.active || revision !== this.revision) {
        return;
      }
      this.failure = errorMessage(error as BusinessError);
    }
    if (!this.active || revision !== this.revision) {
      return;
    }
    this.refreshDevices();
    this.timer = setInterval(() => { this.tick(); }, 16);
  }

  stop(): void {
    this.active = false;
    ++this.revision;
    if (this.timer >= 0) {
      clearInterval(this.timer);
      this.timer = -1;
    }
    nativeClient.stopTransport();
    const session = this.session;
    this.session = undefined;
    this.writing = false;
    if (session) {
      session.disconnect();
    }
  }

  private refreshDevices(): void {
    const devices: ClientDevice[] = [];
    try {
      if (!this.authorized || access.getState() !== access.BluetoothState.STATE_ON) {
        this.onStatus(this.failure);
      } else {
        for (const address of connection.getPairedDevices().slice(0, 512)) {
          let name = address;
          try {
            name = connection.getRemoteDeviceName(address);
          } catch (error) {
            this.onStatus(`Failed to read device name: ${errorMessage(error as BusinessError)}`);
          }
          devices.push({ address: address, name: name });
        }
        this.onStatus(`${devices.length} paired devices · Classic RFCOMM`);
      }
    } catch (error) {
      this.onStatus(`Device enumeration failed: ${errorMessage(error as BusinessError)}`);
    }
    nativeClient.setDevices(devices);
  }

  private tick(): void {
    if (!this.active) {
      return;
    }
    try {
      const snapshot = nativeClient.pollTransport(!this.writing);
      if (snapshot.refreshDevices) {
        this.refreshDevices();
      }
      if (snapshot.action !== 0) {
        const previous = this.session;
        this.session = undefined;
        this.writing = false;
        this.generation = snapshot.generation;
        if (previous) {
          previous.disconnect();
        }
        if (snapshot.action === 1) {
          this.open(snapshot);
        }
        return;
      }
      if (snapshot.data.byteLength > 0) {
        const session = this.session;
        if (!session || this.generation !== snapshot.generation) {
          nativeClient.transportFailed(snapshot.generation, 'RFCOMM transport unavailable');
          return;
        }
        this.write(session, snapshot);
      }
    } catch (error) {
      this.onStatus(`Transport failed: ${errorMessage(error as BusinessError)}`);
      this.stop();
    }
  }

  private open(snapshot: TransportSnapshot): void {
    const revision = this.revision;
    const generation = snapshot.generation;
    let lastMessage = 'RFCOMM disconnected';
    const session: RfcommSession = new RfcommSession(
      (state: SessionState) => {
        if (!this.active || revision !== this.revision || generation !== this.generation || this.session !== session) {
          return;
        }
        if (state === SessionState.CONNECTED) {
          nativeClient.transportConnected(generation);
        } else if (state === SessionState.DISCONNECTED) {
          nativeClient.transportFailed(generation, lastMessage);
        }
      },
      (message: string) => {
        if (this.active && revision === this.revision && this.session === session &&
          !message.startsWith('RX ') && !message.startsWith('TX ')) {
          lastMessage = message;
          this.onStatus(message);
        }
      },
      (data: ArrayBuffer) => {
        if (this.active && revision === this.revision && this.session === session) {
          nativeClient.feedRx(generation, data);
        }
      }
    );
    this.session = session;
    try {
      if (!this.authorized || access.getState() !== access.BluetoothState.STATE_ON) {
        throw new Error(this.failure);
      }
      session.connect(snapshot.address, snapshot.uuid, true);
    } catch (error) {
      if (this.active && revision === this.revision && this.session === session) {
        nativeClient.transportFailed(generation, errorMessage(error as BusinessError));
        this.session = undefined;
      }
      session.disconnect();
    }
  }

  private async write(session: RfcommSession, snapshot: TransportSnapshot): Promise<void> {
    this.writing = true;
    try {
      await session.write(snapshot.data);
    } catch (error) {
      if (this.active && this.session === session && this.generation === snapshot.generation) {
        nativeClient.transportFailed(snapshot.generation, errorMessage(error as BusinessError));
      }
    } finally {
      if (this.session === session && this.generation === snapshot.generation) {
        this.writing = false;
      }
    }
  }
}
