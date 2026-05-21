import sys
import argparse
from deadlock import DL_Extension

def main():
    parser = argparse.ArgumentParser(description='DeadLock Package Manager')
    subparsers = parser.add_subparsers(dest='command', help='Available commands')

    # Create project command
    init_parser = subparsers.add_parser('create', help='Create and Initialize a new project')
    init_parser.add_argument('projectName', help='Name of the project')

    # Install packages command
    install_parser = subparsers.add_parser('install', help='Install packages')
    install_parser.add_argument('packages', nargs='+', help='Packages to install')

    # Sync command
    subparsers.add_parser('sync', help='Sync packages from dead.lock file')

    # List command
    subparsers.add_parser('list', help='List installed packages')

    # Is package installed command
    subparsers.add_parser('show', help='Shows details about the package if installed')

    # Get package info command
    info_parser = subparsers.add_parser('info', help='Gets info about a package from pypi.')
    info_parser.add_argument('packageName', help='Package Name to get info of.')

    # Uninstall Packages command
    uninstall_parser = subparsers.add_parser('uninstall', help='Uninstalls package(s) from virtual environment')
    uninstall_parser.add_argument('packages', nargs='+', help='Package(s) to be uninstalled.')

    args = parser.parse_args()

    try:
        dl = DL_Extension()

        if args.command == 'create':
            print(f"Creating project {args.projectName}...")
            dl.create_project(args.projectName)

        elif args.command == 'install':
            success = dl.install_packages(args.packages)
            if success:
                print(f"Successfully installed: {', '.join(args.packages)}")
            else:
                print("Installation failed!", file=sys.stderr)
                sys.exit(1)

        elif args.command == 'sync':
            success = dl.sync_fromdeadlock()
            if success:
                print("Synced packages from dead.lock file")
            else:
                print("Sync failed!", file=sys.stderr)
                sys.exit(1)

        elif args.command == 'list':
            packages = dl.get_installed_packages()
            if packages:
                print("Installed packages:")
                for pkg in packages:
                    print(f"  - {pkg.name} ({pkg.version})")
            else:
                print("No packages installed")
        elif args.command == 'show':
            success = dl.is_installed()
            if success:
                print("Package found in dead.lock file.")
            else:
                print("Could not find package in dead.lock file")
                sys.exit(1)
        elif args.command == 'uninstall':
            success = dl.uninstall_packages(args.packages)
            if success:
                print(f"Successfully uninstalled: {', '.join(args.packages)}")
            else:
                print("Uninstallation failed: ", file=sys.stderr)
                sys.exit(1)
        else:
            parser.print_help()

    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
