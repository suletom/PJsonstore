/*

PJSONSTORE class (main object):

pj_elem_data& get_by_num(int)
 
void empty() Deletes everthing int the object

void unset(); Same as empty
 
int get_size() Gets number of elements
 
int has_key(string | int) Check whether the given key exists
 
int get_type() Always returns: PJT_ARRAY = 0 or PJT_NULL on new instance
 
void add(string | int | empty)  Adds an array/object to the "list" like php's $array[] operator, with key or with index.

int is_indexed() 

to_json(String)
 
int from_json(String)


 
pj_elem_data class (sub elements):

 
pj_elem_data& get_by_num(int) 

void empty() Makes the array/object empty, not affecting its parent key -> type: PJT_NULL size: 0 
 
void unset() Deletes the element with its key, if any 
 
int get_size() Gets number of elements

int has_key(string | int)
 
int get_type() Gets type:  PJT_NULL = 6, PJT_BOOLEAN = 3, PJT_INTEGER = 2, PJT_FLOAT = 5(not working),  PJT_DOUBLE = 4,  PJT_STRING = 1, PJT_ARRAY = 0,

void add(string | int | empty)  Adds an array/object to the "list" like php's $array[] operator, with key or with index.

int get_index() Get numerical index, if not exist -1 is returned

char * get_key() Get char * pointer to the key string   !! WARNING: this pointer gets freed if the object is destroyed

void create() Creates an empty array

_pj_value_holder get_value() Gets value struct, can be accessed like (example: get_value().pj_string (string) get.value().pj_bool (boolean) )

 */

#ifndef PJSONSTORE_h
#define PJSONSTORE_h

#ifndef pjsontest
#include "Arduino.h"
#endif

#include <stdio.h>
#include <string.h>
#ifdef pjsontest
#include <string>
#endif
#include <stdlib.h>
#include <stdarg.h>
//#include <typeinfo>
#include <limits.h>
#include <errno.h>
#include "jsmn.h"

#define PJ_MAX_NUMERIC_INDEX_CHARS 10

using namespace std;

#ifdef pjsontest

class String : public string {
public:

    String()
    : string() {
    }

    String(const char * s)
    : string(s) {
    }

};
#endif

class _pjBase {
public:

    _pjBase() {
    }

    virtual ~_pjBase() {
    }

protected:
    virtual void _destroy() = 0;
};

typedef enum __pj_elem_type {
    PJT_ARRAY = 0,
    PJT_STRING = 1,
    PJT_INTEGER = 2,
    PJT_BOOLEAN = 3,
    PJT_DOUBLE = 4,
    //PJT_FLOAT = 5,
    PJT_NULL = 6

} _pj_elem_type;

typedef struct __pj_value_holder {
    int index;
    char* key;
    _pj_elem_type type;

    union {
        char* pj_string;
        int pj_int;
        bool pj_bool;
        double pj_double;
        //float pj_float;
        _pjBase* pj_array;
    };

} _pj_value_holder;

class pj_elem_data : public _pjBase {
public:

    pj_elem_data(void * parent);

    pj_elem_data(pj_elem_data& pr);

    void empty();

    void unset();

    int get_size();

    int to_json(String &s);

    int has_key(const char * str);

    int has_key(int ind);

    int is_indexed();

    pj_elem_data& get_by_num(int i);

    void create(char obj_or_arr);

    pj_elem_data& add();

    pj_elem_data& add(const int index);

    pj_elem_data& add(const char* key);

    pj_elem_data& operator=(pj_elem_data& pr);

    pj_elem_data& operator=(_pjBase* pr);

    pj_elem_data& operator=(const char* pr);

    pj_elem_data& operator=(String* pr);

    pj_elem_data& operator=(const int& pr);

    pj_elem_data& operator=(const bool& pr);

    pj_elem_data& operator=(const float& pr);

    pj_elem_data& operator=(const double& pr);

    pj_elem_data& operator[](const int index);

    pj_elem_data& operator[](const char* key);

    void set_index(int ind);

    int get_index();

    char* get_key();

    int set_key(const char* key);

    _pj_elem_type get_type();

    _pj_value_holder get_value();

    virtual ~pj_elem_data() {
        free(pj_intholder.key);
        _destroy();
    }

protected:

    virtual void _destroy() {

        if (pj_intholder.type == PJT_ARRAY) {
            delete (pj_intholder.pj_array);
        }

        if (pj_intholder.type == PJT_STRING) {
            free(pj_intholder.pj_string);
        }

        pj_intholder.type = PJT_NULL;

    }

private:
    _pj_value_holder pj_intholder;
    void * pj_parent;

};

class pjsonstore : public _pjBase {
private:
    pj_elem_data** pj_childs;
    int pj_childnumber;
    static String * outbuf;
    pjsonstore * pj_parent;
    char pj_numkeys;
    int pj_max;

public:

    pjsonstore() {
        pj_childs = NULL;
        pj_childnumber = 0;
        pj_numkeys = 0;
        pj_max = -1;
    }

    pjsonstore(char numkeys) {
        pj_childs = NULL;
        pj_childnumber = 0;
        if (numkeys) {
            pj_numkeys = 1;
        }
        pj_max = -1;
    }

    virtual ~pjsonstore() {
        empty();
    }

    void empty() {
        _destroy();
    }

    void unset() {
        _destroy();
    }

    int get_type() {
        return PJT_ARRAY;
    }

    pj_elem_data& operator[](const int index) {
        return intopfunc(index);
    }

    pj_elem_data& operator[](const char* key) {
        return charopfunc(key);
    }

    pj_elem_data& add() {
        return intopfunc(pj_max + 1);
    }

    pj_elem_data& add(const int index) {
        return intopfunc(index);
    }

    pj_elem_data& add(const char* key) {
        return charopfunc(key);
    }

    int get_size() {
        return pj_childnumber;
    }

    int get_max() {
        return pj_max;
    }

    int has_key(const char * str) {
        if (find_item(str) != NULL) return 1;
        return 0;
    }

    int has_key(int i) {
        char tmp[PJ_MAX_NUMERIC_INDEX_CHARS];
        sprintf(tmp, "%d", i);
        if (find_item(tmp) != NULL) return 1;
        return 0;
    }

    int is_indexed() {
        return (pj_numkeys && pj_childnumber - 1 == pj_max);
    }

    pj_elem_data& get_by_num(int i) {
        return (*(pj_childs[i]));
    }

    void to_json(String &s) {
        pjsonstore::outbuf = &s;
        _to_json();
    }

    /**
     * Success: 1
     * Fail: 0 
     */
    int from_json(int tokens, const char * buf) {
        int r;
        jsmn_parser p;
        jsmntok_t t[tokens]; /* We expect no more than 128 tokens */
        int current_token = 0;
        int errorflag = 1;

        jsmn_init(&p);
        r = jsmn_parse(&p, buf, strlen(buf), t, sizeof (t) / sizeof (t[0]));
        if (r < 0) {
            return 0;
        }
        if (r < 1 || (t[0].type != JSMN_OBJECT && t[0].type != JSMN_ARRAY)) {
            return 0;
        }

        current_token = 0;
        _json_parse(&errorflag, t, buf, &r, &current_token, this, NULL);
        if (errorflag != 1) {
            return 0;
        }
        return 1;
    }

    void remove(pj_elem_data * it) {
        int i = -1;
        char tmpc[PJ_MAX_NUMERIC_INDEX_CHARS];
        char indexed = 0;
        char ind_ok = 1;
        int ind = it->get_index();

        for (i = 0; i < pj_childnumber; i++) {
            if (it == pj_childs[i]) {

                break;
            }
        }

        if (i >= 0) {

            if (pj_numkeys && pj_childnumber - 1 == pj_max) {
                indexed = 1;
            }

            pj_elem_data** tmp = (pj_elem_data**) malloc((pj_childnumber - 1) * sizeof (pj_elem_data*));
            memcpy(tmp, pj_childs, i * sizeof (pj_elem_data*));
            memcpy(tmp + i, pj_childs + i + 1, (pj_childnumber - i - 1) * sizeof (pj_elem_data*));
            delete pj_childs[i];
            free(pj_childs);
            pj_childs = tmp;

            pj_childnumber--;
            if (indexed) {

                //not the best solution...
                ind++;
                while (ind - 1 < pj_childnumber) {
                    for (int j = 0; j < pj_childnumber; j++) {
                        if (tmp[j]->get_index() == ind) {
                            sprintf(tmpc, "%d", tmp[j]->get_index() - 1);
                            tmp[j]->set_key(tmpc);
                            ind++;
                        }
                    }
                }
                pj_max = pj_max - 1;
            } else {
                //check if the list becomes indexed
                ind = 0;
                for (int g = 0; g < pj_childnumber; g++) {
                    if (tmp[g]->get_index() == ind) {
                        ind++;
                    } else {
                        ind_ok = 0;
                    }
                }

                if (ind_ok) {
                    pj_numkeys = 1;
                }

            }

        }

    }

protected:

    virtual void _destroy() {

        if (pj_childnumber != 0) {
            for (int i = 0; i < pj_childnumber; i++) {
                delete pj_childs[i];
            }
            free(pj_childs);
            pj_childnumber = 0;
            pj_max = -1;
            pj_numkeys = 0;
            pj_childs = NULL;
        }

    }

    //if array is indexed with continous numeric keys

    void set_indexed(int newindex) {
        if (pj_numkeys == 0 && newindex >= 0) {
            //printf("0 -> 1");
            pj_numkeys = 1;
        }

        if (pj_numkeys == 1) {

            if (newindex == -1) {
                //printf("1 -> 0");
                pj_numkeys = 0;
            }

        }

        if (newindex >= 0 && pj_max < newindex) {
            pj_max = newindex;
        }

    }

    pj_elem_data& intopfunc(const int index) {
        char tmp[PJ_MAX_NUMERIC_INDEX_CHARS];
        sprintf(tmp, "%d", index);

        pj_elem_data* pItem = find_item(tmp);
        if (pItem != NULL) {
            return (*pItem);
        } else {

            pj_childnumber++;
            pj_childs = (pj_elem_data**) realloc(pj_childs, pj_childnumber * sizeof (pj_elem_data*));
            pj_childs[pj_childnumber - 1] = new pj_elem_data(this);
            set_indexed(pj_childs[pj_childnumber - 1]->set_key(tmp));

            return (*(pj_childs[pj_childnumber - 1]));
        }
    }

    pj_elem_data* find_item(int i) {
        char tmp[PJ_MAX_NUMERIC_INDEX_CHARS];
        sprintf(tmp, "%d", i);
        return find_item(tmp);
    }

    pj_elem_data* find_item(const char* key) {
        int iIndex = -1;

        for (int i = 0; i < pj_childnumber; i++) {
            if (strcmp(pj_childs[i]->get_key(), key) == 0) {
                iIndex = i;
                break;
            }
        }

        if (iIndex != -1) {
            return (pj_childs[iIndex]);
        } else {
            return NULL;
        }
    }

    pj_elem_data& charopfunc(const char* key) {

        pj_elem_data* pItem = find_item(key);

        if (pItem != NULL) {
            return (*pItem);
        } else {
            pj_childnumber++;
            pj_childs = (pj_elem_data**) realloc(pj_childs, pj_childnumber * sizeof (pj_elem_data*));
            pj_childs[pj_childnumber - 1] = new pj_elem_data(this);
            set_indexed(pj_childs[pj_childnumber - 1]->set_key(key));

            return (*(pj_childs[pj_childnumber - 1]));
        }
    }

    void pj_print(const char c) {
        //printf("%c",c);

        if (pjsonstore::outbuf == NULL) {
            //printf("NULL\n");
        } else {
            (*pjsonstore::outbuf) += c;
        }

    }

    void pj_print(const char * s) {
        //printf("%s",s);

        if (pjsonstore::outbuf == NULL) {
            //printf("NULL\n");
        } else {
            (*pjsonstore::outbuf) += s;
        }

    }

    void pj_simprintf(const char * format, ...) {

        va_list args;
        va_start(args, format);
        int size = vsnprintf(NULL, 0, format, args);
        va_end(args);

        char * tmp;

        tmp = (char *) malloc((sizeof (char)*(size + 1)));
        va_start(args, format);
        vsnprintf(tmp, (size + 1), format, args);
        va_end(args);

        *(tmp + size) = '\0';

        pj_print(tmp);

        free(tmp);

    }

    void _to_json() {

        bool is_ind_arr = 0;
        pj_elem_data * elem;

        if (pj_numkeys && pj_childnumber - 1 == pj_max) {
            is_ind_arr = 1;
        }

        if (is_ind_arr) {
            pj_print("[");
        } else {
            pj_print("{");
        }

        for (int i = 0; i < pj_childnumber; i++) {


            if (is_ind_arr) {
                elem = this->find_item(i);
                elem = pj_childs[i];
            } else {
                elem = pj_childs[i];
            }

            if (is_ind_arr) {

            } else {

                _escape_string(elem->get_key());
                pj_print(":");

            }

            switch (elem->get_type()) {
                case PJT_NULL:
                    pj_print("null");
                    break;
                case PJT_BOOLEAN:

                    if (elem->get_value().pj_bool) {
                        pj_print("true");
                    } else {
                        pj_print("false");
                    }

                    break;
                case PJT_INTEGER:

                    pj_simprintf("%d", elem->get_value().pj_int);
                    break;
                /*case PJT_FLOAT:

                    pj_simprintf("%g", elem->get_value().pj_float);

                    break;
                 */ 
                case PJT_DOUBLE:

                    pj_simprintf("%g", elem->get_value().pj_double);

                    break;
                case PJT_STRING:

                    _escape_string(elem->get_value().pj_string);

                    break;
                case PJT_ARRAY:
                    ((pjsonstore&) (*(elem)->get_value().pj_array))._to_json();
                    break;
            }

            if (i < pj_childnumber - 1) {

                pj_print(",");

            }

        }

        if (is_ind_arr) {

            pj_print("]");

        } else {

            pj_print("}");

        }

    }

    void _escape_string(char * str) {

        pj_print("\"");

        for (long j = 0; j < strlen(str); ++j) {
            switch (*(str + j)) {
                case '"':

                    pj_print("\\\"");
                    break;
                case '/':

                    pj_print("\\/");
                    break;
                case '\b':

                    pj_print("\\b");
                    break;
                case '\f':

                    pj_print("\\f");
                    break;
                case '\n':

                    pj_print("\\n");
                    break;
                case '\r':

                    pj_print("\\r");
                    break;
                case '\t':

                    pj_print("\\t");
                    break;
                case '\\':

                    pj_print("\\\\");
                    break;
                default:

                    /* NO UTF-8 support yet
                    if (*(str+j) >= 0x00 && *(str+j) <= 0x1f) {
                        // print character c as \uxxxx
                        pj_simprintf("u%04x",int(*(str+j)) );
                            
                    }else {
                     */
                    pj_print(*(str + j));
                    //}    
                    break;
            }
        }

        pj_print("\"");
    }

    //process 1 array/object/primitive

    void _json_parse(int * rv, jsmntok_t * t, const char * buf, int * tokens_count, int * current_token, pjsonstore * list, char * key) {

        char * tmpstring;
        int max;
        pj_elem_data * iti;

        if (*(current_token) >= *(tokens_count)) {
            return;
        }

        /*printf("\nprocess type: ");
        switch (t[*(current_token)].type){
            case JSMN_UNDEFINED:
                printf("JSMN_UNDEFINED");
            break;
            case JSMN_OBJECT:
                printf("JSMN_OBJECT");
            break;
            case JSMN_ARRAY:
                printf("JSMN_ARRAY");
            break;
            case JSMN_STRING:
                printf("JSMN_STRING");
            break;
            case JSMN_PRIMITIVE:
                printf("JSMN_PRIMITIVE");
            break;
        }        
            
        printf(" at token: %d size: %d PTS=> start: %d end: %d  (tokens count: %d parent type: %d)\n",*(current_token),t[*(current_token)].size,t[*(current_token)].start,t[*(current_token)].end,*(tokens_count), t[t[*(current_token)].parent].type);
         */

        //printf("for i: %d/%d\n",i,max-1);

        //workaround for a known bug in jsmn: only a string in an object is not valid
        if (t[t[*(current_token)].parent].type == JSMN_OBJECT) {
            if (t[*(current_token)].type == JSMN_STRING && (t[*(current_token)].size > 0)) {

            } else {
                *(rv) = 0;
                return;
            }
        }

        switch (t[*(current_token)].type) {
            case JSMN_UNDEFINED:
                *(current_token) = *(current_token) + 1;
                (*rv) = 0;

                break;
            case JSMN_OBJECT:
            case JSMN_ARRAY:

                if (t[*(current_token)].type == JSMN_ARRAY) {
                    if (*(current_token) != 0) {
                        if (t[*(current_token)].size == 0) {
                            //create an empty array where we are
                            if (key != NULL) {
                                (*list).add(key) = new pjsonstore(1);
                                key = NULL;
                            } else {
                                (*list).add() = new pjsonstore(1);
                            }
                        } else {
                            if (key != NULL) {
                                iti = &((*list).add(key));
                                (*iti) = new pjsonstore(1);
                                list = (pjsonstore *) ((*iti).get_value().pj_array);
                                //printf("keynull\n");
                                key = NULL;
                            } else {
                                iti = &((*list).add());
                                (*iti) = new pjsonstore(1);
                                list = (pjsonstore *) ((*iti).get_value().pj_array);
                            }
                        }

                    } else {
                        (*list).pj_numkeys = 1;
                    }
                }

                if (t[*(current_token)].type == JSMN_OBJECT) {
                    if (*(current_token) != 0) {
                        if (t[*(current_token)].size == 0) {
                            //create an empty object where we are
                            (*list).add() = new pjsonstore(0);
                        } else {
                            if (key != NULL) {
                                iti = &((*list).add(key));
                                (*iti) = new pjsonstore(0);
                                list = (pjsonstore *) ((*iti).get_value().pj_array);
                                //printf("keynull\n");
                                key = NULL;
                            } else {
                                iti = &((*list).add());
                                (*iti) = new pjsonstore(0);
                                list = (pjsonstore *) ((*iti).get_value().pj_array);
                            }
                        }
                    }
                }

                max = t[*(current_token)].size;
                *(current_token) = *(current_token) + 1;
                //printf("m: %d\n",max);                        
                for (int i = 0; i < max; i++) {

                    _json_parse(rv, t, buf, tokens_count, current_token, list, key);

                }

                break;
            case JSMN_STRING:

                tmpstring = (char *) malloc(sizeof (char)*((t[*(current_token)].end)-(t[*(current_token)].start) + 1));
                strncpy(tmpstring, buf + (t[*(current_token)].start), ((t[*(current_token)].end)-(t[*(current_token)].start)));
                *(tmpstring + (t[*(current_token)].end)-(t[*(current_token)].start)) = '\0';

                //string value
                if (t[*(current_token)].size == 0) {

                    *(current_token) = *(current_token) + 1;
                    if (key != NULL) {
                        (*list).add(key) = tmpstring;
                        key = NULL;
                    } else {
                        (*list).add() = tmpstring;
                    }

                } else {

                    *(current_token) = *(current_token) + 1;
                    _json_parse(rv, t, buf, tokens_count, current_token, list, tmpstring);
                }
                free(tmpstring);

                break;
            case JSMN_PRIMITIVE:

                tmpstring = (char *) malloc(sizeof (char)*((t[*(current_token)].end)-(t[*(current_token)].start) + 1));
                strncpy(tmpstring, buf + (t[*(current_token)].start), ((t[*(current_token)].end)-(t[*(current_token)].start)));
                *(tmpstring + (t[*(current_token)].end)-(t[*(current_token)].start)) = '\0';

                if (add_value(tmpstring, list, key) == 0) {
                    (*rv) = 0;
                }

                *(current_token) = *(current_token) + 1;
                free(tmpstring);

                break;
        }

    }

    /**
     * Success: 1
     * Fail: 0 
     */
    int add_value(char * tmpstring, pjsonstore * list, char * key) {

        pj_elem_data * iti;

        if (key != NULL) {
            iti = &((*list).add(key));
            key = NULL;
        } else {
            iti = &((*list).add());
        }

        //char tmpstring[]="3.4";
        //check types: boolean, null, int, double
        if (strcmp(tmpstring, "true") == 0) {
            (*iti) = true;
            return 1;
        }
        if (strcmp(tmpstring, "false") == 0) {
            (*iti) = false;
            return 1;
        }
        if (strcmp(tmpstring, "null") == 0) {
            //its ok, null is already there
            return 1;
        }

        errno = 0;
        char* end = tmpstring;

        long int result = strtol(tmpstring, &end, 10);
        if (errno == ERANGE) {
            //printf("Error: integer overflow\n");
        } else if (*end) {
            //printf("Error: not a valid integer\n");
        } else {
            if (result > INT_MAX || result < INT_MIN) {
                //printf("Error: valid number, but not fits into int\n");
            } else {
                (*iti) = (int) result;
                return 1;
            }

            //printf("Result is: %ld",result);
        }

        /* disabled: source of problems...
        end = tmpstring;
        errno = 0;
        float f = strtof(tmpstring, &end);
        if (errno == ERANGE) {
        } else if (*end) {
        } else {
            (*iti) = f;
            return 1;
        }
        */

        end = tmpstring;
        errno = 0;
        double d = strtod(tmpstring, &end);
        if (errno == ERANGE) {

        } else if (*end) {
        } else {
            (*iti) = d;
            return 1;
        }

        return 0;
    }


};

String * pjsonstore::outbuf = NULL;

pj_elem_data::pj_elem_data(void * parent) {
    pj_parent = parent;
    memset(&pj_intholder, 0, sizeof (_pj_value_holder));
    pj_intholder.type = PJT_NULL;
    pj_intholder.index = -1;
}

pj_elem_data::pj_elem_data(pj_elem_data& pr) {
    pj_parent = NULL;
    pj_intholder.type = pr.get_type();

    int len = 0;
    char* pj_string = NULL;

    switch (pj_intholder.type) {
        case PJT_ARRAY:
            pj_intholder.pj_array = pr.get_value().pj_array;
            break;
        case PJT_STRING:
            pj_string = pr.get_value().pj_string;
            len = strlen(pj_string) + 1;
            pj_intholder.pj_string = (char*) malloc(len);
            strncpy(pj_intholder.pj_string, pj_string, len - 1);
            pj_intholder.pj_string[len - 1] = '\0';
            break;
        case PJT_INTEGER:
            pj_intholder.pj_int = pr.get_value().pj_int;
            break;
        case PJT_BOOLEAN:
            pj_intholder.pj_bool = pr.get_value().pj_bool;
            break;
        /*case PJT_FLOAT:
            pj_intholder.pj_float = pr.get_value().pj_float;
            break;
         */
        case PJT_DOUBLE:
            pj_intholder.pj_double = pr.get_value().pj_double;
            break;
        default:
            memset(&pj_intholder, 0, sizeof (_pj_value_holder));
            pj_intholder.type = PJT_NULL;
            break;
    }
}

void pj_elem_data::set_index(int ind) {
    pj_intholder.index = ind;
}

int pj_elem_data::get_index() {
    return pj_intholder.index;
}

void pj_elem_data::empty() {
    _destroy();
}

char* pj_elem_data::get_key() {
    return (pj_intholder.key);
}

int pj_elem_data::set_key(const char* key) {
    int ind;

    int isdigit = 0;
    int j = 0;
    while (j < strlen(key) && isdigit == 0) {
        if (key[j] > 57 && key[j] < 48)
            isdigit = 0;
        else
            isdigit = 1;
        j++;
    }

    if (isdigit == 1 && sscanf(key, "%d", &ind) == 1 && ind >= 0) {

        pj_intholder.index = ind;

    }

    int len = strlen(key) + 1;
    pj_intholder.key = (char*) realloc(pj_intholder.key, len * sizeof (char));
    strcpy(pj_intholder.key, key);
    pj_intholder.key[len - 1] = '\0';

    return ((isdigit == 1 && sscanf(key, "%d", &ind) == 1 && ind >= 0) ? ind : -1);

}

_pj_elem_type pj_elem_data::get_type() {
    return (pj_intholder.type);
}

_pj_value_holder pj_elem_data::get_value() {
    return (pj_intholder);
}

pj_elem_data& pj_elem_data::add(const int index) {
    if (pj_intholder.type != PJT_ARRAY) {

        _destroy();

        pj_intholder.type = PJT_ARRAY;
        pj_intholder.pj_array = new pjsonstore();

        return (((pjsonstore&) (*pj_intholder.pj_array))[index]);
    }

    return (((pjsonstore&) (*pj_intholder.pj_array))[index]);
}

void pj_elem_data::create(char obj_or_arr) {

    if (pj_intholder.type != PJT_ARRAY) {

        _destroy();

        pj_intholder.type = PJT_ARRAY;
        pj_intholder.pj_array = new pjsonstore(obj_or_arr);

    }

}

pj_elem_data& pj_elem_data::add() {

    if (pj_intholder.type != PJT_ARRAY) {

        _destroy();

        pj_intholder.type = PJT_ARRAY;
        pj_intholder.pj_array = new pjsonstore();

        return (((pjsonstore&) (*pj_intholder.pj_array))[0]);
    }

    int s = ((pjsonstore&) (*pj_intholder.pj_array)).get_max();
    s++;
    return (((pjsonstore&) (*pj_intholder.pj_array))[s]);

}

pj_elem_data& pj_elem_data::operator[](const int index) {
    return add(index);
}

pj_elem_data& pj_elem_data::operator=(pj_elem_data& pr) {

    _destroy();

    pj_intholder.type = pr.get_type();

    char* pj_string = NULL;
    int len = 0;

    switch (pj_intholder.type) {

        case PJT_ARRAY:

            pj_intholder.pj_array = pr.get_value().pj_array;
            break;

        case PJT_STRING:

            pj_string = pr.get_value().pj_string;
            len = strlen(pj_string) + 1;
            pj_intholder.pj_string = (char*) malloc(len);
            strncpy(pj_intholder.pj_string, pj_string, len - 1);
            pj_intholder.pj_string[len - 1] = '\0';
            break;

        case PJT_INTEGER:
            pj_intholder.pj_int = pr.get_value().pj_int;
            break;

        case PJT_BOOLEAN:
            pj_intholder.pj_bool = pr.get_value().pj_bool;
            break;
        /*    
        case PJT_FLOAT:
            pj_intholder.pj_float = pr.get_value().pj_float;
            break;
         */ 

        case PJT_DOUBLE:
            pj_intholder.pj_double = pr.get_value().pj_double;
            break;

        default:
            memset(&pj_intholder, 0, sizeof (_pj_value_holder));
            pj_intholder.type = PJT_NULL;
            break;

    }

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(_pjBase* pr) {

    _destroy();
    pj_intholder.type = PJT_ARRAY;
    pj_intholder.pj_array = pr;

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(const char* pr) {

    _destroy();
    pj_intholder.type = PJT_STRING;
    int len = strlen(pr) + 1;
    pj_intholder.pj_string = (char*) malloc(len);
    strncpy(pj_intholder.pj_string, pr, len - 1);
    pj_intholder.pj_string[len - 1] = '\0';

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(String* pr) {

    _destroy();
    pj_intholder.type = PJT_STRING;
    int len = strlen(pr->c_str()) + 1;
    pj_intholder.pj_string = (char*) malloc(len);
    strncpy(pj_intholder.pj_string, pr->c_str(), len - 1);
    pj_intholder.pj_string[len - 1] = '\0';

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(const int& pr) {

    _destroy();
    pj_intholder.type = PJT_INTEGER;
    pj_intholder.pj_int = pr;

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(const bool& pr) {

    _destroy();
    pj_intholder.type = PJT_BOOLEAN;
    pj_intholder.pj_bool = pr;

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(const float& pr) {

    _destroy();
    pj_intholder.type = PJT_DOUBLE;
    pj_intholder.pj_double = pr;

    return (*this);
}

pj_elem_data& pj_elem_data::operator=(const double& pr) {

    _destroy();
    pj_intholder.type = PJT_DOUBLE;
    pj_intholder.pj_double = pr;

    return (*this);
}

pj_elem_data& pj_elem_data::add(const char* key) {
    if (pj_intholder.type != PJT_ARRAY) {

        _destroy();

        pj_intholder.type = PJT_ARRAY;
        pj_intholder.pj_array = new pjsonstore();

        return (((pjsonstore&) (*pj_intholder.pj_array))[key]);
    }

    return (((pjsonstore&) (*pj_intholder.pj_array))[key]);
}

pj_elem_data& pj_elem_data::operator[](const char* key) {
    return add(key);
}

int pj_elem_data::get_size() {

    if (pj_intholder.type == PJT_ARRAY) {
        return ((pjsonstore&) (*pj_intholder.pj_array)).get_size();
    }
    return 0;
}

int pj_elem_data::has_key(const char * str) {

    if (pj_intholder.type == PJT_ARRAY) {
        return ((pjsonstore&) (*pj_intholder.pj_array)).has_key(str);
    }
    return 0;
}

int pj_elem_data::has_key(int ind) {

    if (pj_intholder.type == PJT_ARRAY) {
        return ((pjsonstore&) (*pj_intholder.pj_array)).has_key(ind);
    }
    return 0;
}

pj_elem_data& pj_elem_data::get_by_num(int i) {

    if (pj_intholder.type == PJT_ARRAY) {
        return ((pjsonstore&) (*pj_intholder.pj_array)).get_by_num(i);
    }
}

int pj_elem_data::to_json(String &s) {

    if (pj_intholder.type == PJT_ARRAY) {
        ((pjsonstore&) (*pj_intholder.pj_array)).to_json(s);
    }
}

void pj_elem_data::unset() {
    //unset in parent list ???
    ((pjsonstore *) pj_parent)->remove(this);
}

int pj_elem_data::is_indexed() {
    if (pj_intholder.type == PJT_ARRAY) {
        return ((pjsonstore&) (*pj_intholder.pj_array)).is_indexed();
    }
    return 0;
}


#endif /* PJSONSTORE_h */
