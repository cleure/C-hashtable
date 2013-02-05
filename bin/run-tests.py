#!/usr/bin/python

import os, sys, subprocess

TEST_BIN_PATH = 'tests/bin'

def main():
    tests = [
        os.path.join(TEST_BIN_PATH, file)
            for file in os.listdir(TEST_BIN_PATH) if file[0] != '.'
    ]
    
    for test in tests:
        assert_test = [test]
        memory_test = ['valgrind', '--leak-check=full', '--error-exitcode=6', test]
        
        print('Runnin Assert test for %s' % (test))
        p = subprocess.Popen(assert_test)
        p.communicate()
        if p.returncode != 0:
            print('Assert test failed for %s with code %d' % (test, p.returncode))
            print('Quitting')
            sys.exit(6)
        
        print('Assert test successful, no errors')
        print('Runnin Memory test for %s' % (test))
        p = subprocess.Popen(memory_test)
        p.communicate()
        if p.returncode != 0:
            print('Memory test failed for %s' % (test))
            print('Quitting')
            sys.exit(6)
        
        print('Memory test successful, no errors')
    
    print('All tests successful')
    sys.exit(0)
    
if __name__ == '__main__':
    main()
