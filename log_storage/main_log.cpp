/*======================================================== 
**University of Illinois/NCSA 
**Open Source License 
**
**Copyright (C) 2007-2008,The Board of Trustees of the University of 
**Illinois. All rights reserved. 
**
**Developed by: 
**
**    Research Group of Professor Sam King in the Department of Computer 
**    Science The University of Illinois at Urbana-Champaign 
**    http://www.cs.uiuc.edu/homes/kingst/Research.html 
**
**Permission is hereby granted, free of charge, to any person obtaining a 
**copy of this software and associated documentation files (the 
**��Software��), to deal with the Software without restriction, including 
**without limitation the rights to use, copy, modify, merge, publish, 
**distribute, sublicense, and/or sell copies of the Software, and to 
**permit persons to whom the Software is furnished to do so, subject to 
**the following conditions: 
**
*** Redistributions of source code must retain the above copyright notice, 
**this list of conditions and the following disclaimers. 
*** Redistributions in binary form must reproduce the above copyright 
**notice, this list of conditions and the following disclaimers in the 
**documentation and/or other materials provided with the distribution. 
*** Neither the names of <Name of Development Group, Name of Institution>, 
**nor the names of its contributors may be used to endorse or promote 
**products derived from this Software without specific prior written 
**permission. 
**
**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
**EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
**MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
**IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
**ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
**TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
**SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE. 
**========================================================== 
*/
#include <iostream>
#include <string>
#include <assert.h>

#include "LogStorage.h"
#include "../kernel/kernel.h"

using namespace std;

int main(int argc, char **argv){
    Message msg;
    LogStorage store;
    bool loop = true;

    while(loop) {
        try {
            msg.readMessage(STDIN_FILENO);
            if(msg.getDstId() != AUDITLOG_ID) {
                store.putMessage(&msg);
            } else if(msg.getMsgType() == MSG_GET_OBJECT_AUDIT_RECORDS){
                store.initGetObjectAuditRecords(msg.getMsgValue());
                while(store.getNextMessage(&msg)) {
                    msg.writeMessage(STDOUT_FILENO);
                }
                msg.queryAuditLogRet(STDOUT_FILENO);
            }
        } catch(...) {
            loop = false;
        }
    }

    return 0;
}