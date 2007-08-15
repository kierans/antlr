/** \file
 * Definition of the ANTLR3 base tree adaptor.
 */

#ifndef	_ANTLR3_BASE_TREE_ADAPTOR_H
#define	_ANTLR3_BASE_TREE_ADAPTOR_H

#include    <antlr3defs.h>
#include    <antlr3collections.h>
#include    <antlr3string.h>
#include    <antlr3basetree.h>
#include    <antlr3commontoken.h>
#include	<antlr3debugeventlistener.h>


typedef	struct ANTLR3_BASE_TREE_ADAPTOR_struct
{
    /** Pointer to any enclosing structure/interface that
     *  contains this structure.
     */
    void							* super;

    /** We need a string factory for creating imaginary tokens, we take this
     *  from the stream we are supplied to walk.
     */
    pANTLR3_STRING_FACTORY			strFactory;

    /* And we also need a token factory for creating imaginary tokens
     * this is also taken from the input source.
     */
    pANTLR3_TOKEN_FACTORY			tokenFactory;

	/// If set to something other than NULL, then this structure is
	/// points to an instance of the debugger interface. In general, the
	/// debugger is only referenced internally in recovery/error operations
	/// so that it does not cause overhead by having to check this pointer
	/// in every function/method
	///
	pANTLR3_DEBUG_EVENT_LISTENER	debugger;

    pANTLR3_BASE_TREE	    (*nil)					(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor);

    pANTLR3_BASE_TREE	    (*dupTree)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE tree);

    void					(*addChild)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t, pANTLR3_BASE_TREE child);
    void					(*addChildToken)		(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t, pANTLR3_COMMON_TOKEN child);

    pANTLR3_BASE_TREE	    (*becomeRoot)			(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE newRoot, pANTLR3_BASE_TREE oldRoot);

    pANTLR3_BASE_TREE	    (*rulePostProcessing)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE root);

    pANTLR3_BASE_TREE	    (*becomeRootToken)		(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_COMMON_TOKEN newRoot, pANTLR3_BASE_TREE oldRoot);

    pANTLR3_BASE_TREE	    (*create)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adpator, pANTLR3_COMMON_TOKEN payload);
    pANTLR3_BASE_TREE	    (*createTypeToken)		(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, ANTLR3_UINT32 tokenType, pANTLR3_COMMON_TOKEN fromToken);
    pANTLR3_BASE_TREE	    (*createTypeTokenText)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, ANTLR3_UINT32 tokenType, pANTLR3_COMMON_TOKEN fromToken, pANTLR3_UINT8 text);
    pANTLR3_BASE_TREE	    (*createTypeText)		(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, ANTLR3_UINT32 tokenType, pANTLR3_UINT8 text);

    pANTLR3_BASE_TREE	    (*dupNode)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE treeNode);

    ANTLR3_UINT32			(*getType)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t);

    void					(*setType)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t, ANTLR3_UINT32 type);
    
    pANTLR3_STRING			(*getText)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t);

    void					(*setText)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_STRING t);
    void					(*setText8)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_UINT8 t);

    pANTLR3_BASE_TREE	    (*getChild)				(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, ANTLR3_UINT64 i);

    pANTLR3_UINT64			(*getChildCount)		(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE);

    ANTLR3_UINT64			(*getUniqueID)			(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE);

    pANTLR3_COMMON_TOKEN    (*createToken)			(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, ANTLR3_UINT32 tokenType, pANTLR3_UINT8 text);
    pANTLR3_COMMON_TOKEN    (*createTokenFromToken)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_COMMON_TOKEN fromToken);

    void					(*setTokenBoundaries)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t, pANTLR3_COMMON_TOKEN startToken, pANTLR3_COMMON_TOKEN stopToken);

    ANTLR3_UINT64			(*getTokenStartIndex)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t);

    ANTLR3_UINT64			(*getTokenStopIndex)	(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor, pANTLR3_BASE_TREE t);

    void					(*free)					(struct ANTLR3_BASE_TREE_ADAPTOR_struct * adaptor);

}
    ANTLR3_TREE_ADAPTOR;

#endif
