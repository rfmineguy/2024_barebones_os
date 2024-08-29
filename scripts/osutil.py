import argparse
import os
import sys
import subprocess
import docker

client = docker.from_env()

class OsUtilParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2)

def is_container_running(container_name):
    return client.containers.get(container_name).status == 'running'

def grub_ops(args):
    print(client.containers.get('os-build'))
    if not is_container_running('os-build'):
        client.containers.run('ubuntu-limbarebones', name='os-build')

    if args.createcfg:
        print('create grub.cfg')
    if args.checkmultiboot:
        container = client.containers.get('os-build')
        r = container.exec_run('sh -c "if grub-file --is-x86-multiboot out/os.bin; then echo Multiboot Ok; else Multiboot Not Ok; fi"', tty=True)
        print(r)
    if args.containerrunning:
        print(is_container_running('os-build'))

    pass

parser = OsUtilParser()
subparsers = parser.add_subparsers(dest='subparser', help='subcommand help')

parser_grub = subparsers.add_parser('grub', help='Manages docker related operations')
parser_grub.add_argument('--create-cfg', dest='createcfg', action='store_true', help='Creates basic grub config structure')
parser_grub.add_argument('--check-multiboot', dest='checkmultiboot', action='store_true', help='Checks if the binary is multiboot compatible')
parser_grub.add_argument('--is-container-running', dest='containerrunning', action='store_true', help='Check if the container is running')

args = parser.parse_args()
if args.subparser == 'grub':
    grub_ops(args)
