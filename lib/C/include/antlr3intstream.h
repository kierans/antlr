/** \file
 * Defines the the class interface for an antlr3 INTSTREAM.
 * 
 * Certain funcitonality (such as DFAs for instance) abstract the stream of tokens
 * or characters in to a steam of integers. Hence this structure should be included
 * in any stream that is able to provide the output as a stream of integers (which is anything
 * basically.
 *
 * There are no specific implementations of the methods in this interface in general. Though
 * for purposes of casting and so on, it may be necesssary to implement a function with
 * the signature in this interface which abstracts the base immplementation. In essence though
 * the base stream provides a pointer to this interface, within which it installs its
 * normal match() functions and so on. Interaces such as DFA are then passed the pANTLR3_INT_STREAM
 * and can treat any input as an int stream. 
 *
 * For instance, a lexer implements a pANTLR3_BASE_RECOGNIZER, within which there is a pANTLR3_INT_STREAM.
 * However, a pANTLR3_INPUT_STREAM also provides a pANTLR3_INT_STREAM, which it has constructed from
 * it's normal interface when it was created. This is then pointed at by the pANTLR_BASE_RECOGNIZER
 * when it is intialized with a pANTLR3_INPUT_STREAM.
 *
 * Similarly if a pANTLR3_BASE_RECOGNIZER is initialized with a pANTLR3_TOKEN_STREAM, then the 
 * pANTLR3_INT_STREAM is taken from the pANTLR3_TOKEN_STREAM. 
 *
 * If a pANTLR3_BASE_RECOGNIZER is initialized with a pANTLR3_TREENODE_STREAM, then guess where
 * the pANTLR3_INT_STREAM comes from?
 *
 * Note that because the context pointer points to the actual interface structure that is providing
 * the ANTLR3_INT_STREAM it is defined as a (void *) in this interface. There is no direct implementation
 * of an ANTLR3_INT_STREAM (unless someone did not understand what I was doing here =;?P
 */
#ifndef	_ANTLR3_INTSTREAM_H
#define	_ANTLR3_INTSTREAM_H

#include    <antlr3defs.h>
#include    <antlr3commontoken.h>

/** Type inidicator for a character stream
 * \remark if a custom stream is created but it can be treated as
 * a char stream, then you may OR in this value to your type indicator
 */
#define	ANTLR3_CHARSTREAM	0x0001

/** Type indicator for a Token stream
 * \remark if a custom stream is created but it can be treated as
 * a token stream, then you may OR in this value to your type indicator
 */
#define	ANTLR3_TOKENSTREAM	0x0002

/** Type indicator for a common tree node stream
 * \remark if a custom stream is created but it can be treated as
 * a common tree node stream, then you may OR in this value to your type indicator
 */
#define	ANTLR3_COMMONTREENODE	0x0004

/** Type mask for input stream so we can switch in the above types
 *  \remark DO NOT USE 0x0000 as a stream type!
 */
#define	ANTLR3_INPUT_MASK	0x0007

typedef	struct ANTLR3_INT_STREAM_struct
{
    /** Input stream type inidicator. Sometimes useful for error reporting etc.
     */
    ANTLR3_UINT32	    type;

    /** Whatever is providing this interface needs a pointer to itself
     *  so that this can be passed back to it whenever the api functions
     *  are called.
     */
    void		    * me;

    /** Special token for a token stream to restun EOF by
     */
    pANTLR3_COMMON_TOKEN    eofToken;
    
    /** If set to ANTLR3_TRUE then the input stream has an exception
     * condition (this is tested by the generated code for the rules of
     * the grammar).
     */
    ANTLR3_BOOLEAN	    error;

    /** Points to the first in a possible chain of exceptions that the
     *  recognizer has discovered.
     */
    pANTLR3_EXCEPTION	    exception;

    /** Pointer to a funtion that can construct a generic exception structure
     * with such information as the input stream can privide.
     */
    void		    (*exConstruct)  (pANTLR3_INT_STREAM intStream);

    /** Consume the next 'ANTR3_UINT32' in the stream
     */
    void		    (*consume)	    (void * intStream);

    /** Get ANTLR3_UINT32 at current input pointer + i ahead where i=1 is next ANTLR3_UINT32 
     */
    ANTLR3_UINT32	    (*LA)	    (void * intStream, ANTLR3_INT64 i);

    /** Tell the stream to start buffering if it hasn't already.  Return
     *  current input position, index(), or some other marker so that
     *  when passed to rewind() you get back to the same spot.
     *  rewind(mark()) should not affect the input cursor.
     */
    ANTLR3_UINT64	    (*mark)	    (void * intStream);
    
    /** Return the current input symbol index 0..n where n indicates the
     *  last symbol has been read.
     */
    ANTLR3_INT64	    (*index)	    (void * intStream);

    /** Reset the stream so that next call to index would return marker.
     *  The marker will usually be index() but it doesn't have to be.  It's
     *  just a marker to indicate what state the stream was in.  This is
     *  essentially calling release() and seek().  If there are markers
     *  created after this marker argument, this routine must unroll them
     *  like a stack.  Assume the state the stream was in when this marker
     *  was created.
     */
    void		    (*rewind)	    (void * intStream, ANTLR3_UINT64 marker);

    /** You may want to commit to a backtrack but don't want to force the
     *  stream to keep bookkeeping objects around for a marker that is
     *  no longer necessary.  This will have the same behavior as
     *  rewind() except it releases resources without the backward seek.
     */
    void		    (*release)	    (void * intStream, ANTLR3_UINT64 mark);

    /** Set the input cursor to the position indicated by index.  This is
     *  normally used to seek ahead in the input stream.  No buffering is
     *  required to do this unless you know your stream will use seek to
     *  move backwards such as when backtracking.
     *
     *  This is different from rewind in its multi-directional
     *  requirement and in that its argument is strictly an input cursor (index).
     *
     *  For char streams, seeking forward must update the stream state such
     *  as line number.  For seeking backwards, you will be presumably
     *  backtracking using the mark/rewind mechanism that restores state and
     *  so this method does not need to update state when seeking backwards.
     *
     *  Currently, this method is only used for efficient backtracking, but
     *  in the future it may be used for incremental parsing.
     */
    void		    (*seek)	    (void * intStream, ANTLR3_UINT64 index);

    /** Only makes sense for streams that buffer everything up probably, but
     *  might be useful to display the entire stream or for testing.
     */
    ANTLR3_UINT64	    (*size)	    (void * intStream);

    /** Frees any resources that were allocated for the implementation of this
     *  interface. Usually this is just releasing the memory allocated
     *  for the structure itself, but it may of course do anything it need to
     *  so long as it does not stamp on anything else.
     */
    void		    (*free)	    (struct ANTLR3_INT_STREAM_struct * stream);

}
    ANTLR3_INT_STREAM;

#endif
