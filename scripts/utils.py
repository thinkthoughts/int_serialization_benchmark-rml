"""
Shared utility functions for benchmark scripts.
"""

import subprocess
import platform
import os


def get_cpu_model():
    """Get the CPU model name for the current system."""
    env = os.environ.copy()
    env["LANG"] = "C"

    system = platform.system()
    if system == "Windows":
        return platform.processor()
    elif system == "Darwin":
        os.environ['PATH'] += os.pathsep + '/usr/sbin'
        command = ["sysctl", "-n", "machdep.cpu.brand_string"]
        return subprocess.check_output(command, env=env, text=True).strip()
    elif system == "Linux":
        output = subprocess.check_output(["lscpu"], env=env, text=True)
        model_name = None
        architecture = None
        for line in output.splitlines():
            if "Model name:" in line:
                model_name = line.split(":", 1)[1].strip()
            elif "Architecture:" in line:
                architecture = line.split(":", 1)[1].strip()
        return model_name or architecture or "unknown_cpu"
    return "unknown_cpu"
