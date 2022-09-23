#!/usr/bin/env python3
import argparse
import socket
import struct
import abc


class ServerAction(abc.ABC):
    def __init__(self, dst_ip, dst_port):
        super().__init__()
        self._remote_addr = (dst_ip, dst_port)

    @abc.abstractclassmethod
    def get_action_name(self):
        pass
    
    @abc.abstractclassmethod
    def get_action_num(self):
        pass
    
    def perform_action(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect(self._remote_addr)
            sock.send(self.get_payload())

    @abc.abstractmethod
    def get_payload(self) -> bytes:
        pass

    def _create_buffer(self, data):
        return struct.pack("<i", self.get_action_num()) + struct.pack("N", len(data)) + data
    
    @classmethod
    def get_all_actions(cls):
        return cls.__subclasses__()
    
    @classmethod
    def parse_action(cls, action_name, *args):
        try:
            return {i.get_action_name(): i for i in cls.get_all_actions()}.get(action_name)(*args)\

        except TypeError:
            raise argparse.ArgumentTypeError("expected more arguments for the '{}' action".format(action_name))


class CloseAction(ServerAction):
    @classmethod
    def get_action_name(cls):
        return "close"
    
    @classmethod
    def get_action_num(cls):
        return 1
    
    def get_payload(self) -> bytes:
        return self._create_buffer(b"")


class ExecAction(ServerAction):
    def __init__(self, dst_ip, dst_port, bin_path):
        super().__init__(dst_ip, dst_port)
        self._bin_path = bin_path

    @classmethod
    def get_action_name(cls):
        return "exec"
    
    @classmethod
    def get_action_num(cls):
        return 0
    
    def get_payload(self) -> bytes:
        with open(self._bin_path, 'rb') as fd:
            return self._create_buffer(fd.read())


def main():
    parser = argparse.ArgumentParser(description='Communicates with a remote \
                                                  server that is able to run \
                                                  given binaries')
                                                  
    parser.add_argument('action', 
                        choices=[i.get_action_name() for i in ServerAction.get_all_actions()])
    parser.add_argument('--ip', required=True)
    parser.add_argument('--port', required=True, type=int)
    parser.add_argument('rest', nargs=argparse.REMAINDER)

    args = parser.parse_args()

    action = ServerAction.parse_action(args.action, args.ip, args.port, *args.rest)
    action.perform_action()


if __name__ == "__main__":
    main()
