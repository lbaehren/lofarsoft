# Simple test program for ep_echo Python module
import ep_echo

if __name__ == "__main__":
    ep_interface = ep_echo.EP_Interface()
    for i in range(10):
        print i, ": ", ep_interface.time_echo(), "seconds"

