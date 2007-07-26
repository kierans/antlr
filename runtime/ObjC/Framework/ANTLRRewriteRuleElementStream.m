// [The "BSD licence"]
// Copyright (c) 2006-2007 Kay Roepke
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import "ANTLRRewriteRuleElementStream.h"


@implementation ANTLRRewriteRuleElementStream

- (id) initWithTreeAdaptor:(id<ANTLRTreeAdaptor>)aTreeAdaptor description:(NSString *)anElementDescription
{
    return [self initWithTreeAdaptor:aTreeAdaptor description:anElementDescription element:nil];
}

- (id) initWithTreeAdaptor:(id<ANTLRTreeAdaptor>)aTreeAdaptor description:(NSString *)anElementDescription element:(id)anElement
{
    self = [super init];
    if (self) {
        [self setDescription:anElementDescription];
        isSingleElement = YES;
        elements.single = [anElement retain];
    }
    return self;
}

- (id) initWithTreeAdaptor:(id<ANTLRTreeAdaptor>)aTreeAdaptor description:(NSString *)anElementDescription elements:(NSArray *)theElements
{
    self = [super init];
    if (self) {
        [self setDescription:anElementDescription];
        isSingleElement = NO;
        elements.multiple = [[NSMutableArray alloc] initWithArray:theElements];
    }
    return self;
}

- (void) dealloc
{
    if (isSingleElement)
        [elements.single release];
    else
        [elements.multiple release];
    [self setDescription:nil];
    [super dealloc];
}

- (void) reset
{
    cursor = 0;
}

- (void) addElement: (id)anElement
{
    if (isSingleElement) {
        
        if (elements.single == nil) {
            elements.single = [anElement retain];
            return;
        }
        
        isSingleElement = NO;
        NSMutableArray *newArray = [[NSMutableArray alloc] initWithCapacity:5];
        [newArray addObject:elements.single];
        [elements.single release];  // balance previous retain in initializer/addElement
        [newArray addObject:anElement];
        elements.multiple = newArray;
    } else {
        [elements.multiple addObject:anElement];
    }
}

- (unsigned int) count
{
    if (isSingleElement && elements.single != nil)
        return 1;
    if (isSingleElement == NO && elements.multiple != nil)
        return [elements.multiple count];
    return 0;
}


- (BOOL) hasNext
{
    return (isSingleElement && elements.single != nil && cursor < 1) ||
            (elements.multiple != nil && cursor < [elements.multiple count]);
}

- (id) next
{
    if (cursor>=[self count] && [self count]==1) {
        id element = [self _next];
        return [self copyElement:element];
    }
    id element = [self _next];
    return element;
}

- (id) _next       // internal: TODO: redesign if necessary. maybe delegate
{
    if ([self count] == 0) {
        @throw [NSException exceptionWithName:@"RewriteEmptyStreamException" reason:nil userInfo:nil];// TODO: fill in real exception
    }
    if ( cursor >= [self count] ) {
        if ( [self count] == 1 ) {
            return [self toTree:elements.single];
        }
        @throw [NSException exceptionWithName:@"RewriteCardinalityException" reason:nil userInfo:nil];// TODO: fill in real exception
    }
    if (isSingleElement && elements.single != nil) {
        cursor++;
        return [self toTree:elements.single];
    }
    id el = [self toTree:[elements.multiple objectAtIndex:cursor]];
    cursor++;
    return el;
}

- (id) copyElement:(id)element
{
    [self doesNotRecognizeSelector:_cmd];   // subclass responsibility
    return nil;
}

- (id) toTree:(id)element
{
    return element;
}


- (NSString *) description
{
    return elementDescription;
}

- (void) setDescription:(NSString *) description
{
    if (description != elementDescription) {
        [elementDescription release];
        elementDescription = [description retain];
    }
}



@end