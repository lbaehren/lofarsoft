# Simple test program for ep_echo Python module
import ep_echo

if __name__ == "__main__":
    ep_echo.ep_init("lfe001")
    for i in range(10):
        print i, ": ", ep_echo.time_echo(), "seconds"

