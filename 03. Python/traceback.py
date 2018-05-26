import traceback 


try:
    print "Hello, Try \n"
    raise Exception("Exception")
except:
    error = open('errorInfo.txt', 'w')
    error.write(traceback.format_exc())
    error.close()
    
