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

export const setScale: (scale: number) => void;
export const pollTransport: (takeTx: boolean) => TransportSnapshot;
export const transportConnected: (generation: number) => void;
export const transportFailed: (generation: number, error: string) => void;
export const feedRx: (generation: number, data: ArrayBuffer) => void;
export const setDevices: (devices: ClientDevice[]) => void;
export const stopTransport: () => void;
