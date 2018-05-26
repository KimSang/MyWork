import re

SampleText = "phoneNumber E-Mail Address 010-1234-1234 Hello@Hello.net HaHaHa@HaHaHa.neeeeet SampleText 010-1111-1111" 

phoneNumber = re.compile(r'''(
                            (\d{2,3}) 
                            (\s|-|\.)?
                            (\d{3,4})
                            (\s|-|\.)
                            (\d{4})
                            )''', re.VERBOSE)

email = re.compile(r'''(
                       [a-zA-Z0-9._%+-]+
                        @
                        [a-zA-Z0-9.-]+
                        (\.[a-zA-Z]{2,4})
                        )''', re.VERBOSE)

mo_email = email.search(SampleText)

if mo_email != None:
    print "E-Mail : " + mo_email.group()

mo_find_all = phoneNumber.findall(SampleText)

if mo_find_all != None:
    for phone in mo_find_all:
        print "Phone : " + str(phone[0])
