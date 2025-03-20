import os, sys, subprocess, configparser, time

class deployer:
    def __init__(self):
        self.get_paths()
        self.plugin_dir = os.getcwd()

    def get_paths(self) -> str:
        # Read the configuration file for 'Destination' directory
        config = configparser.ConfigParser()
        
        config.read('Tools\VersionManager\config.ini')
        self.destination = config['Paths']['Destination']
        self.ver = config['Version']['Engine']
        self.engine_dir = config['Engine'][self.ver]

    def remove_folder(self, dst:str):
        try:
            process = subprocess.Popen(['rmdir', '/S', '/Q', dst], shell=True)
            time.sleep(1)

            # Check if the process is still running
            if process.poll() is None:
                print(f"Process is still running, terminating it.")
                # Terminate the process if it's still running
                process.terminate()  # or process.kill() for forceful termination
            else:
                print(f"Process finished with return code {process.returncode}")

        except subprocess.CalledProcessError as e:
            print(f"Error occurred while trying to remove directory {dst}: {e}")

    def deploy_development(self):        
        if not os.path.exists(self.destination):
            # throw an error then exit
            print('Destination directory does not exist. Exiting...')
            return

        dst = os.path.join(self.destination, os.path.basename(self.plugin_dir))

        if os.path.exists(self.destination):
            self.remove_folder(dst)

        # make a soft link
        try:
            process = subprocess.Popen(['mklink', '/J', dst, self.plugin_dir], shell=True)
            time.sleep(2)

            # Check if the process is still running
            if process.poll() is None:
                print(f"Process is still running, terminating it.")
                # Terminate the process if it's still running
                process.terminate()
        except  subprocess.CalledProcessError as e:
            print(f"Error occurred while trying to create soft link {dst}: {e}")

    def deploy_precompiled(self):
        if not os.path.exists(self.destination):
            # throw an error then exit
            print('Destination directory does not exist. Exiting...')
            return
        
        dst = os.path.join(self.destination, os.path.basename(self.plugin_dir))

        if os.path.exists(self.destination):
            self.remove_folder(dst)

        # copy the plugin directory to the destination
        try:
            process = subprocess.Popen(['robocopy', self.plugin_dir, dst, '/E'], shell=True)
            time.sleep(2)

            # Check if the process is still running
            if process.poll() is None:
                print(f"Process is still running, terminating it.")
                # Terminate the process if it's still running
                process.terminate()
        except subprocess.CalledProcessError as e:
            print(f"Error occurred while trying to copy directory {dst}: {e}")

        # remove the .git directory
        git_dir = os.path.join(dst, '.git')
        if os.path.exists(git_dir):
            print(f"Removing folder {git_dir}")
            self.remove_folder(git_dir)

        # remove the .gitignore file
        gitignore_file = os.path.join(dst, '.gitignore')
        if os.path.exists(gitignore_file):
            print(f"Removing file {gitignore_file}")
            os.remove(gitignore_file)

        # remove the .vscode directory
        vscode_dir = os.path.join(dst, '.vscode')
        if os.path.exists(vscode_dir):
            print(f"Removing folder {vscode_dir}")
            self.remove_folder(vscode_dir)

        # remove Tools directory
        tools_dir = os.path.join(dst, 'Tools')
        if os.path.exists(tools_dir):
            print(f"Removing folder {tools_dir}")
            self.remove_folder(tools_dir)
        
        # remove .pdb files under dst\Binaries\Win64\
        binaries_dir = os.path.join(dst, 'Binaries', 'Win64')
        for f in os.listdir(binaries_dir):
            file = os.path.join(binaries_dir, f)
            if file.endswith('.pdb'):
                print(f"Removing file {file}")
                os.remove(file)

        source_dir = os.path.join(dst, 'Source')
        for subdir in os.listdir(source_dir):
            fldr = os.path.join(source_dir, subdir)
            print(fldr)

            # remove Private directory
            private_dir = os.path.join(fldr, 'Private')
            if os.path.exists(private_dir):
                print(f"Removing folder {private_dir}")
                self.remove_folder(private_dir)

            # set .Build.cs file bUsePrecompiled = true
            build_file = os.path.join(fldr, subdir + '.Build.cs')
            print(f"Set build file {build_file} bUsePrecompiled = true")
            with open(build_file, 'r') as file:
                lines = file.readlines()
                for i, line in enumerate(lines):
                    if 'bUsePrecompiled' in line:
                        lines[i] = lines[i].replace('// ', '')
                file.close()
            # write the changes back to the file
            with open(build_file, 'w') as file:
                file.writelines(lines)
                file.close()

    def recompile_project(self):
        # generate project.uproject file
        unreal_build_tool = os.path.join(self.engine_dir, 'Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe')

        # project dir is self.destination's parent directory
        project_dir = os.path.dirname(self.destination)
        project_file = os.path.join(project_dir, f'{os.path.basename(project_dir)}.uproject')

        try:
            process = subprocess.Popen([unreal_build_tool, '-projectfiles', f'-project={project_file}', '-game', '-engnie'], shell=True)
            time.sleep(15)

            # Check if the process is still running
            if process.poll() is None:
                print(f"Process is still running, terminating it.")
                # Terminate the process if it's still running
                process.terminate()
        except subprocess.CalledProcessError as e:
            print(f"Error occurred while trying to generate project files: {e}")

def main(arg):
    plugin_deployer = deployer()

    if arg == 'dev':
        plugin_deployer.deploy_development()
    elif arg == 'pre':
        plugin_deployer.deploy_precompiled()

    plugin_deployer.recompile_project()

if __name__ == '__main__':
    if len(sys.argv) != 2 or sys.argv[1] not in ['dev', 'pre']:
        print('Usage: python DeployPlugin.py <dev|pre>')
        sys.exit()

    main(sys.argv[1])