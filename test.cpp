/* 
 * File:   simpletest.cpp
 * Author: suletom
 *
 * Created on 2019. augusztus 6., 16:43
 */

#include <stdlib.h>
#include <iostream>
#define pjsontest 1
#include "PJsonstore.h"


void test(int test_number,int tokens,const char * jsonstring,int retval,int check_same){

    std::cout << "%TEST_STARTED% test" << test_number << " (simpletest)" << std::endl;
    
    pjsonstore a;
    String s;
    
    
    int rv = a.from_json(tokens,jsonstring);
    
    //std::cout << "rv: " << rv << endl;
    
    if (rv) {
        if (retval) {
            
        }else{
            std::cout << "%TEST_FAILED% time=0 testname=test" << test_number << " (simpletest) message=RETVAL FALSE: NOT AS EXPECTED" << std::endl;
        }
    }else{
        if (retval) {
            std::cout << "%TEST_FAILED% time=0 testname=test" << test_number << " (simpletest) message=RETVAL TRUE NOT AS EXPECTED" << std::endl;
            
        }else{
            
        }
    }
    
    a.to_json(s);
    
    //std::cout << "Genereted json: " << s << endl;
    
    if (check_same && s!=String(jsonstring)) {
        std::cout << "%TEST_FAILED% time=0 testname=test" << test_number << " (simpletest) message=RESULT_NOT_EQUAL_WITH_INPUT -> " << String(jsonstring) << " != " << s << std::endl;
        
    }
    
    //std::cout << "%TEST_FAILED% time=0 testname=test" << test_number << " (simpletest) message=msg" << std::endl;
    
    
    std::cout << "%TEST_FINISHED% time=0 test" << test_number << " (simpletest)" << std::endl;
    
}

int main(int argc, char** argv) {
    int i=0;
    
    std::cout << "%SUITE_STARTING% simpletest" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;
    
    /* parsing tests */
    
    //empty object
    test(i++,128,"{}",1,1);
    
    //empty array
    test(i++,128,"[]",1,1);
    
    //only one string in object
    test(i++,128,"{\"string\"}",0,0);
    
    //standard pair
    test(i++,128,"{\"key\":\"value\"}",1,1);
    
    //malformed json
    test(i++,128,"{\"key:\"value\"}",0,0);
    
    //empty array element
    test(i++,128,"{\"key\":[]}",1,1);
    
    //array element with 1 string
    test(i++,128,"{\"key\":[\"string\"]}",1,1);
    
    //array element with 2 objects
    test(i++,128,"{\"key\":[{},{}]}",1,1);
    
    //array element with 2 objects
    test(i++,128,"{\"key\":[{\"key\":\"val\"},{\"key1\":\"val1\"}]}",1,1);
    
    //not enough memory
    test(i++,6,"{\"key\":[{\"key\":\"val\"},{\"key1\":\"val1\"}]}",0,0);
    
    //missing ]
    test(i++,128,"{\"key\":[{\"key\":\"val\"},{\"key1\":\"val1\"}}",0,0);
    
    //missing }
    test(i++,128,"{\"key\":[{\"key\":\"val\"},{\"key1\":\"val1\"}]",0,0);
    
    //int and float primitives
    test(i++,128,"{\"key\":[{\"key\":3},{\"key1\":3.3}]}",1,1);
    
    //empty array in object
    test(i++,128,"{\"key\":\"value\",[]}",0,0);
    
    //1 string in array
    test(i++,128,"[\"as\"]",1,1);
   
    //primitives in array string first
    test(i++,128,"[\"as\",1,1.2,null,false,true]",1,1);
    
    //primitives in array value first
    test(i++,128,"[1,\"as\",1.2,null,false,false]",1,1);
    
    //not empty alist!!! Ok OR NOT?
    
    //set indexed flags on not empty list, or any case
    
    std::cout << "%SUITE_FINISHED% time=0" << std::endl;
     
    return (EXIT_SUCCESS);
}

