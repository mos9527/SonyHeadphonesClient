export interface ClientDevice {
  address: string;
  name: string;
}

export interface TransportSnapshot {
  generation: number;
  action: number;
  refreshDevices: boolean;
  address: string;
  uuid: string;
  data: ArrayBuffer;
}

export interface ShareRequest {
  id: number;
  filename: string;
  mimeType: string;
  data: ArrayBuffer;
}

export const setShareAvailable: (available: boolean) => void;
export const pollShare: () => ShareRequest | undefined;
export const completeShare: (id: number, result: number, error: string) => void;
export const setScale: (scale: number) => void;
export const pollTransport: (takeTx: boolean) => TransportSnapshot;
export const transportConnected: (generation: number) => void;
export const transportFailed: (generation: number, error: string) => void;
export const feedRx: (generation: number, data: ArrayBuffer) => void;
export const setDevices: (devices: ClientDevice[]) => void;
export const stopTransport: () => void;
