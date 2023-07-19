import platform
import subprocess
import configparser

def main():

    config = configparser.ConfigParser()
    config.read('config.ini')


    libraries = config.get('Libraries', 'libraries').split()
    sources = config.get('Sources', 'sources').split()
    ex_name = config.get('Executable','executableName')
    debug = config.get('Executable', 'debug')

    command = 'g++ '

    # Add executable name
    command += f"-o {ex_name} "

    # Add sources
    for source in sources:
        command += f"{source} " 

    # Set STD library to C++ 11
    command += "-std=c++11 "

    # Windows specific flags
    if platform.system() == 'Windows':
        command += "-Iincludes -Llibs "
    
    # Add libraries
    for library in libraries:
        command += f"-l{library} "

    # Set debug
    if debug:
        command += "-g "

    process = subprocess.Popen(["powershell", "-Command", command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()

    # Decode and print the output
    output = output.decode('utf-8')
    error = error.decode('utf-8')

    if output:
        print("Output:")
        print(output)
    if error:
        print("Error:")
        print(error)



if __name__ == '__main__':
    main()
