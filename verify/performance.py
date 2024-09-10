import configparser
import os
import threading

performance = []

def checkLog(logFile):
    numDisplayedMessages = 0
    with open(logFile, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if "Displayed message:" in line:
                numDisplayedMessages += 1
                
    return numDisplayedMessages

times = ["0ms", "50ms", "100ms", "500ms", "1s", "5s", "10s", "50s", "100s", "500s"]
def runTest(config_name, r, time):
    outFile = os.path.join(logPath, config_name + "_" + str(r) + ".log")
    execFile = os.path.join(projectPath, "DS_project")
    print()
    print(f"Running configuration {config_name} with repetition {r}.")
    
    cfg_params = {"Network.linkDelay": times[time]}
    cfg_params = " ".join([f"--{k}={v}" for k, v in cfg_params.items()])
    
    os.system(f'{execFile} -u Cmdenv -f verify/performance.ini -c {config_name} -r {r} {cfg_params}> {outFile}')
    print(f"Checking log of configuration {config_name} with repetition {r}.")
    performance.append((config_name, r, checkLog(outFile)))
    print()


dirPath = os.path.dirname(os.path.abspath(__file__))
projectPath = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
logPath = os.path.join(dirPath, "log")

if not os.path.exists(logPath):
    os.makedirs(logPath)


# Load the INI file
config = configparser.ConfigParser(allow_no_value=True)
config.read(os.path.join(projectPath, 'verify/performance.ini'))

# Initialize a dictionary to store configurations and their repeat values
config_repeats = {}

# Iterate through all sections in the INI file
for section in config.sections():
    repeat_value = config[section].getint('repeat', 1)  # Get the repeat value, default is 1 if not specified
    config_repeats[section] = repeat_value

# Print all configurations and their repeat values
for config_name, repeat in config_repeats.items():
    for r in range(repeat):
        for time in range(10):
            t = threading.Thread(target=runTest, args=(config_name, r, time))
            t.start()
        

print("Waiting for all threads to finish.")
for t in threading.enumerate():
    if t != threading.current_thread():
        t.join()

print(performance)