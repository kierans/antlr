"""ANTLR3 runtime package"""

# [The "BSD licence"]
# Copyright (c) 2005-2006 Terence Parr
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from antlr3.constants import EOF, DEFAULT_CHANNEL, INVALID_TOKEN_TYPE

class Token(object):
    pass

## 	/** Get the text of the token */
## 	public abstract String getText();
## 	public abstract void setText(String text);

## 	public abstract int getType();
## 	public abstract void setType(int ttype);
## 	/**  The line number on which this token was matched; line=1..n */
## 	public abstract int getLine();
##     public abstract void setLine(int line);

## 	/** The index of the first character relative to the beginning of
##          the line 0..n-1 */
## 	public abstract int getCharPositionInLine();
## 	public abstract void setCharPositionInLine(int pos);

## 	public abstract int getChannel();
## 	public abstract void setChannel(int channel);

## 	/** An index from 0..n-1 of the token object in the input stream.
## 	 *  This must be valid in order to use the ANTLRWorks debugger.
## 	 */
## 	public abstract int getTokenIndex();
## 	public abstract void setTokenIndex(int index);


class CommonToken(Token):
    def __init__(self, type=None, channel=DEFAULT_CHANNEL, text=None,
                 input=None, start=None, stop=None, oldToken=None):
        if oldToken is not None:
            self.type = oldToken.type
            self.line = oldToken.line
            self.charPositionInLine = oldToken.charPositionInLine
            self.channel = oldToken.channel
            self.index = oldToken.index
            self._text = oldToken._text
            self.input = oldToken.input
            self.start = oldToken.start
            self.stop = oldToken.stop
            
        else:
            self.type = type
            self.input = input
            self.charPositionInLine = -1 # set to invalid position
            self.line = 0
            self.channel = channel
            
	    #What token number is this from 0..n-1 tokens; < 0 implies invalid index
            self.index = -1
            
            # We need to be able to change the text once in a while.  If
            # this is non-null, then getText should return this.  Note that
            # start/stop are not affected by changing this.
            self._text = text

            # The char position into the input buffer where this token starts
            self.start = start

            # The char position into the input buffer where this token stops
            # This is the index of the last char, *not* the index after it!
            self.stop = stop


    def getText(self):
        if self._text is not None:
            return self._text

        if self.input is None:
            return None
        
        return self.input.substring(self.start, self.stop)


    def setText(self, text):
        """
        Override the text for this token.  getText() will return this text
        rather than pulling from the buffer.  Note that this does not mean
        that start/stop indexes are not valid.  It means that that input
        was converted to a new string in the token object.
	"""
        self._text = text

    text = property(getText, setText)


    def __str__(self):
        channelStr = ""
        if self.channel > 0:
            channelStr = ",channel=" + str(self.channel)

        txt = self.text
        if txt is not None:
            txt = txt.replace("\n","\\\\n")
            txt = txt.replace("\r","\\\\r")
            txt = txt.replace("\t","\\\\t")
        else:
            txt = "<no text>"

        return "[@%s,%s:%s=%r,<%s>%s,%s:%s]" % (
            self.index,
            self.start, self.stop,
            txt,
            self.type, channelStr,
            self.line, self.charPositionInLine
            )
    


EOF_TOKEN = CommonToken(type=EOF)
	
INVALID_TOKEN = CommonToken(type=INVALID_TOKEN_TYPE)

# In an action, a lexer rule can set token to this SKIP_TOKEN and ANTLR
# will avoid creating a token for this symbol and try to fetch another.
SKIP_TOKEN = CommonToken(type=INVALID_TOKEN_TYPE)

