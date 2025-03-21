import os, sys, subprocess, configparser, time

class transaction:   
    def __init__(self, command:list, name:str="transaction", timeout:float=1.0, checking_interval:float=0.05, shell:bool=True):
        self.command = command
        self.timeout = timeout
        self.name = name
        self.checking_interval = checking_interval
        self.shell = shell

    def __call__(self):
        try:
            start_time = time.time()

            process = subprocess.Popen(self.command, shell=self.shell)

            # loop checking every 0.1 seconds if the process is still running, if exceed 15 seconds, terminate it
            while process.poll() is None:
                time.sleep(self.checking_interval)
                if time.time() - start_time > self.timeout:
                    print(f"[{self.name}]: process last over {self.timeout} seconds, terminating it.")
                    process.terminate()
                    break

        except subprocess.CalledProcessError as e:
            print(f"[{self.name}]: Error occurred: {e}")

class deployer:
    def __init__(self):
        self._get_paths()
        self.plugin_dir = os.getcwd()

    def _get_paths(self) -> str:
        # Read the configuration file for 'Destination' directory
        config = configparser.ConfigParser()
        
        config.read('Tools\VersionManager\config.ini')
        self.destination = config['Paths']['Destination']
        self.ver = config['Version']['Engine']
        self.engine_dir = config['Engine'][self.ver]

    def _remove_folder(self, dst:str, seconds:float=1.0):
        command = ['rmdir', '/S', '/Q', dst]
        transaction(command, name="Remove Dir/File (s)", timeout=seconds)()

    def deploy_development(self):        
        if not os.path.exists(self.destination):
            # throw an error then exit
            print('Destination directory does not exist. Exiting...')
            return

        dst = os.path.join(self.destination, os.path.basename(self.plugin_dir))

        if os.path.exists(self.destination):
            self._remove_folder(dst)

        # make a soft link
        command = ['mklink', '/J', dst, self.plugin_dir]
        transaction(command, name=f"Create Soft Link for {dst}", timeout=2)()

    def deploy_precompiled(self):
        if not os.path.exists(self.destination):
            # throw an error then exit
            print('Destination directory does not exist. Exiting...')
            return
        
        dst = os.path.join(self.destination, os.path.basename(self.plugin_dir))

        if os.path.exists(self.destination):
            self._remove_folder(dst)

        # copy the plugin directory to the destination
        command = ['robocopy', self.plugin_dir, dst, '/E']
        transaction(command, name=f"Copy Plugin Directory to {dst}", timeout=2)()

        # remove the .git directory
        git_dir = os.path.join(dst, '.git')
        if os.path.exists(git_dir):
            print(f"Removing folder {git_dir}")
            self._remove_folder(git_dir, seconds=0.5)

        # remove the .gitignore file
        gitignore_file = os.path.join(dst, '.gitignore')
        if os.path.exists(gitignore_file):
            print(f"Removing file {gitignore_file}")
            os.remove(gitignore_file)

        # remove the .vscode directory
        vscode_dir = os.path.join(dst, '.vscode')
        if os.path.exists(vscode_dir):
            print(f"Removing folder {vscode_dir}")
            self._remove_folder(vscode_dir, seconds=0.1)

        # remove Tools directory
        tools_dir = os.path.join(dst, 'Tools')
        if os.path.exists(tools_dir):
            print(f"Removing folder {tools_dir}")
            self._remove_folder(tools_dir, seconds=0.1)
        
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
                self._remove_folder(private_dir, seconds=0.1)

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

        command = [unreal_build_tool, '-projectfiles', f'-project={project_file}', '-game', '-engnie']
        transaction(command, name="Generate Project Files", timeout=15)()

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