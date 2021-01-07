Version 2.2

This package provides generic linked-list manipulation routines.

In addition, queue and stack abstractions are provided by single header files.
To use the package as a list, queue, or stack, simply include the respective
header file and compile with the list library.  Thus, a single set of linked
list primitives can be used to implement stacks, queues, or any other
list-based data structure (such as caches, etc.).

The paper directory contains LaTeX source and DVI and Postscript output from
a paper entitled "The list(3) Package: Software Design and Reusability",
that describes the design and evolution of the package.  This paper appears
in the proceedings of the ACM 30th Annual Southeast Conference, Raleigh,
North Carolina, April 8-10, 1992.

The man page and install targets in the Makefile assume the library, man
pages, and header files will be installed as a piece of system software.
Specifically, the man pages refer to the header file with <>'s instead of
quotes, and the library is referred to with the -l compiler switch.

A set of example programs are provided to illustrate the package.  They
can be compiled without installing (see above) the library.

I have tested this package on Sun3, Sun4, DecStation 5000, PC/RT, HP
9000-400, and Sony(68030, News 3.3) architectures, and it performs
well.  Please notify me of bug reports, comments, or suggestions.

This package is constrained by the GNU Library General Public License as 
published by the Free Software Foundation.  See the file COPYING for details.

Enjoy.

28Jul92
--
```
Bradley C. Spatz                                        Internet:  bcs@ufl.edu
Computer and Information Sciences                    UUCP:  uunet!uflorida!bcs 
College of Engineering
University of Florida	                     "School IS hell" -- Matt Groening
```
# Manpage
```

LIST(3)                    Library Functions Manual                   LIST(3)

NAME
       list_init,   list_mvprev,   list_mvnext,   list_mvfront,  list_mvrear,
       list_front,      list_curr,       list_rear,       list_insert_before,
       list_insert_after,         list_remove_front,        list_remove_curr,
       list_remove_rear, list_size, list_empty, list_traverse,  list_free  --
       generic linked-list routines

SYNOPSIS
       cc [ flags ] files -O2 -llist [ libraries ]

       typedef void (*list_deallocator_t)(void *data);
       typedef list_boolean_t (*list_comparator_t)(void *userdata,
       	       		      			   void *listdata);

       LIST *list_create()
       void list_destroy(LIST *list, list_deallocator_t func);

       size_t list_size(LIST *list);
       list_boolean_t int list_empty(LIST *list);

       void *list_front(LIST *list);
       void *list_curr(LIST *list);
       void *list_rear(LIST *list);

       LIST_ELEMENT *list_element_front(LIST *list);
       LIST_ELEMENT *list_element_curr(LIST *list);
       LIST_ELEMENT *list_element_rear(LIST *list);

       LIST *list_mvprev(LIST *list);
       LIST *list_mvnext(LIST *list);
       LIST *list_mvfront(LIST *list);
       LIST *list_mvrear(LIST *list);
       LIST *list_setcurr(LIST *list, LIST_ELEMENT *element);

       void *list_insert_before(LIST *list, void *data, size_t bytes);
       void *list_insert_after(LIST *list, void *data, size_t bytes);

       void *list_remove_front(LIST *list);
       void *list_remove_curr(LIST *list);
       void *list_remove_rear(LIST *list);

       int list_traverse(LIST *list, void *userdata,
                         list_comparator_t func, int opts);

DESCRIPTION
       These  routines provide generic manipulation of (potentially) multiple
       linked lists. Each list can  hold  arbitrarily  sized  elements,  with
       individual  elements within a list varying in size. It is the program‐
       mer's responsibility to  account  for  such  differences.   Lists  are
       referred  to by variables declared as LIST *; the type LIST is defined
       in <list.h> to be an  opaque  structure.  Internally  each  descriptor
       maintains references to the front, rear, and current elements, as well
       as the size (or length) of the list. Various routines operate relative
       to the current element.

       list_create  creates  and  returns an opaque pointer to the list. This
       pointer is used for all subsequent list operations.

       list_destroy destroys the list, by deallocating the list applying  the
       user-supplied  function list_deallocator_t func to the data portion of
       each element remaining in the list. If func is  (void  *)LIST_DEALLOC,
       then  the  package will apply its own deallocation routine. This, how‐
       ever, should only be done if the package has been responsible for data
       element  allocation,  i.e., the insert routines have been invoked with
       bytes greater than 0. If func is (void *)LIST_NODEALLOC,  no  per-ele‐
       ment deallocation will be performed.

       list_size returns the size (or number of elements) of list as a size_t
       integer.

       list_empty returns 1 (LIST_TRUE) if list is empty, 0 (LIST_FALSE) oth‐
       erwise.

       list_front,  list_curr,  and  list_rear  return pointers to the front,
       current, or rear element in list.

       list_element_front, list_element_curr,  list_element_rear,  return  an
       opaque LIST_ELEMENT pointer. This pointer can only be used to directly
       set the list curr pointer with list_setcurr.   Useful  for  keeping  a
       back pointer to an array element to avoid list traversal.

       list_mvprev,  list_mvnext, list_mvfront, list_mvrear, move to the pre‐
       vious, next, front, or rear element in list, and return  the  modified
       list  descriptor. Movement previous or next is relative to the current
       element.

       list_insert_before and list_insert_after insert an element, pointed to
       by  data and of size bytes, into list, either before or after the cur‐
       rent element. The newly inserted element is then considered  the  cur‐
       rent  element.  Both  routines  return a pointer to the newly inserted
       data. If bytes is greater than 0, then data is copied into  the  list,
       otherwise only the reference data is copied into the list. This allows
       the user to determine the memory allocation policy.

       list_remove_front, list_remove_curr, and list_remove_rear  remove  the
       front,  current, or rear element from list and return a pointer to the
       removed data.  The current element is then set  to  the  next  element
       (prior  to  the remove) in the list, unless the element at the rear of
       the list was removed. In this case, the current element is set to  the
       previous element (prior to the remove).

       list_traverse  traverses  list  according  to  options  opts,  calling
       list_comparator_t func at each element encountered in  the  traversal,
       until  func  returns  0  (FALSE) or the extent of the list is reached.
       This routine can be used to search for an item or print  the  contents
       of  the  list,  for  example.  See the section LIST TRAVERSAL for more
       information.

LIST TRAVERSAL
       The behavior of the routine list_traversal is controlled by the  user-
       supplied  function func which is responsible for the actions performed
       at each element in the traversal. In addition, the scope and direction
       of  the  traversal  can  be  specified  with the opts parameter.  Func
       should be declared as follows:

              list_boolean_t func(void *userdata, void *listdata);

       and should return 1 (LIST_TRUE) if the traversal should continue, or 0
       (LIST_FALSE)  if the traversal should terminate. The function does not
       need to check if the extent of the list has been reached;  the  return
       code  from list_traverse will indicate the status of the traversal. At
       each element encountered in the traversal, list_traverse  will  invoke
       func,  passing  it  the two parameters userdata and listdata; userdata
       being the data pointer provided by the caller in the call to list_tra‐
       verse, and listdata being the data pointer of the current list element
       in the traversal. For example, by passing the  following  function  to
       list_traverse, the contents of a list of names could be printed.

              list_boolean_t func(void *userdata, void *listdata)
              {
                  printf("Name=%30s.\n", (char *)listdata->name);
                  return(LIST_TRUE);
              }

       In  this  example,  the parameter userdata is ignored and the function
       unconditionally returns 1 (LIST_TRUE), but functions like

              list_boolean_t func(void *userdata, void *listdata)
              {
                  if (strcmp((char *)userdata, (char *)listdata))
                     return(LIST_TRUE);
                  else
                     return(LIST_FALSE);
              }

       can be used to position the current element pointer  prior  to  inser‐
       tion, so as to keep the list ordered.

       The direction and scope of the traversal can be controlled by specify‐
       ing one or more of the following options:

              LIST_FORW		* traverse forward (next)
              LIST_BACK 	  traverse backwards (prev)
              LIST_FRNT 	* start from the front of the
                 		  list (implies LIST_FORW)
              LIST_CURR		  start from the current element
              LIST_REAR		  start from the rear element
                 		  (implies LIST_BACK)
              LIST_SAVE 	  * do not alter the current element
                 		  pointer during the traversal
              LIST_ALTR 	  alter the current element pointer
                 		  during the traversal

       The asterisks (*) denote the default values. These options can be com‐
       bined  with  the  logical  OR operator, but at least one value must be
       specified.  For example, specifying

              LIST_FORW

       for opts would request a traversal forwards from the current position,
       restoring the current element pointer after the traversal, whereas

              (LIST_BACK | LIST_CURR | LIST_ALTR)

       would  request  a  traversal  backwards from the current position, and
       would set the current element pointer to the last element  encountered
       in  the traversal.  It should be noted that func should not invoke any
       of the list routines unless LIST_ALTR has been specified,  since  many
       of  the routines act relative to the current element pointer, which is
       not modified during a traversal with LIST_SAVE specified.

MEMORY ALLOCATION
       The  routines  list_init,  list_insert_before,  and  list_insert_after
       allocate  memory  during  their execution. As such, list_insert_before
       and list_insert_after insert a copy of the data  into  the  list  when
       they  are  invoked with bytes greater than 0. If bytes is 0, then only
       the reference is copied into the list. This allows the user to control
       the  memory  allocation  policy. Both functions may fail during memory
       allocation; see DIAGNOSTICS below for  more  information.   Note  that
       list_remove_front, list_remove_curr, and list_remove_rear do not allo‐
       cate memory for the removing data. They simply disassociate  the  data
       from  the  list, and thus return a pointer to data that was previously
       allocated by the package. It is  the  programmer's  responsibility  to
       deallocate  such  removed  data.   If the user has been responsile for
       data element storage allocation, i.e. the insert  routines  have  been
       called  with  bytes  equal to 0, then the user must be responsible for
       storage deallocation as well. A  user-supplied  deallocation  function
       should  be  passed  to list_destroy for this purpose. The deallocation
       function should be declared as:

              void *func(void *data);

       and will be sent the data element reference of  each  element  in  the
       list  when  list_free  is invoked. If the package has been responsible
       for data element allocation,  list_free  can  be  invoked  with  (VOID
       *)LIST_DEALLOC  for func,; the list package will apply its own deallo‐
       cation routine, or (void *)LIST_NODEALLOC if no per-element  dealloca‐
       tion is required. It is the programmer's responsibility to insure that
       the memory allocation policy is applied properly.

DIAGNOSTICS
       A NULL returned by list_init, list_insert_before, or list_insert_after
       indicates  a failure in allocating memory for the new list or element.
       See  malloc(3)  for  more  information.    list_mvprev,   list_mvnext,
       list_mvfront,    list_mvrear,    list_front,   list_curr,   list_rear,
       list_remove_front, list_remove_curr, and list_remove_rear  all  return
       NULL  if list is empty.  list_traverse returns LIST_EMPTY for an empty
       list, LIST_EXTENT if an attempt was made to move beyond the extent  of
       the list, or LIST_OK otherwise.  A core dump indicates a bug ;-)

BUGS
       The  routines  list_remove_front,  list_remove_curr, list_remove_rear,
       and list_free do not physically reclaim storage space,  although  they
       do  make  it available for reuse. While this is a function of free(3),
       its application here could be considered a bug.

SEE ALSO
       queue(3), stack(3), cache(3)

AUTHOR
       Bradley C. Spatz (bcs@ufl.edu), University of Florida.

       Philip Kufeldt

4.3 Berkeley Distribution     September 22, 1991                      LIST(3)
```