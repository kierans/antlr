/*
[The "BSD licence"]
Copyright (c) 2005-2007 Kunle Odutola
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code MUST RETAIN the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form MUST REPRODUCE the above copyright
   notice, this list of conditions and the following disclaimer in 
   the documentation and/or other materials provided with the 
   distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior WRITTEN permission.
4. Unless explicitly state otherwise, any contribution intentionally 
   submitted for inclusion in this work to the copyright owner or licensor
   shall be under the terms and conditions of this license, without any 
   additional terms or conditions.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


namespace Antlr.Runtime.Debug
{
	using System;
	using StreamReader = System.IO.StreamReader;
	using StreamWriter = System.IO.StreamWriter;
	using Encoding = System.Text.Encoding;
	using StringBuilder = System.Text.StringBuilder;
	using TcpClient = System.Net.Sockets.TcpClient;
	using TcpListener = System.Net.Sockets.TcpListener;
	using IToken = Antlr.Runtime.IToken;
	using RecognitionException = Antlr.Runtime.RecognitionException;
	using BaseRecognizer = Antlr.Runtime.BaseRecognizer;
	using ITreeAdaptor = Antlr.Runtime.Tree.ITreeAdaptor;

	/// <summary>
	/// A proxy debug event listener that forwards events over a socket to
	/// debugger (or any other listener) using a simple text-based protocol;
	/// one event per line.
	/// </summary>
	/// <remarks>
	/// ANTLRWorks listens on server socket with a
	/// RemoteDebugEventSocketListener instance.  These two objects must therefore
	/// be kept in sync.  New events must be handled on both sides of socket.
	/// </remarks>
	public class DebugEventSocketProxy : BlankDebugEventListener
	{
		public const int DEFAULT_DEBUGGER_PORT = 0xC001;
		protected int port = DEFAULT_DEBUGGER_PORT;
		protected TcpListener serverSocket;
		protected TcpClient socket;
		protected string grammarFileName;
		protected StreamWriter writer;
		protected StreamReader reader;
		protected BaseRecognizer recognizer;

		/// <summary>
		/// Almost certainly the recognizer will have adaptor set, but
		/// we don't know how to cast it (Parser or TreeParser) to get
		/// the adaptor field.  Must be set with a constructor. :(
		/// </summary>
		protected ITreeAdaptor adaptor;

		public DebugEventSocketProxy(BaseRecognizer recognizer, ITreeAdaptor adaptor)
			: this(recognizer, DEFAULT_DEBUGGER_PORT, adaptor)
		{
		}

		public DebugEventSocketProxy(BaseRecognizer recognizer, int port, ITreeAdaptor adaptor)
		{
			this.grammarFileName = recognizer.GrammarFileName;
			this.port = port;
			this.adaptor = adaptor;
		}

		public virtual void Handshake()
		{
			if (serverSocket == null)
			{
				serverSocket = new TcpListener(port);
				serverSocket.Start();
				socket = serverSocket.AcceptTcpClient();
				socket.NoDelay = true;

				reader = new StreamReader(socket.GetStream(), Encoding.UTF8);
				writer = new StreamWriter(socket.GetStream(), Encoding.UTF8);

				writer.WriteLine("ANTLR " + Constants.DEBUG_PROTOCOL_VERSION);
				writer.WriteLine("grammar \"" + grammarFileName);
				writer.Flush();
			}
		}
		
		public override void  Commence()
		{
			// don't bother sending event; listener will trigger upon connection
		}

		public override void Terminate()
		{
			Transmit("terminate");
			writer.Close();
			try
			{
				socket.Close();
			}
			catch (System.IO.IOException ioe)
			{
				Console.Error.WriteLine(ioe.StackTrace);
			}
		}
		
		protected internal virtual void  Ack()
		{
			try
			{
				reader.ReadLine();
			}
			catch (System.IO.IOException ioe)
			{
				Console.Error.WriteLine(ioe.StackTrace);
			}
		}
		
		protected internal virtual void  Transmit(string eventLabel)
		{
			writer.WriteLine(eventLabel);
			writer.Flush();
			Ack();
		}

		public override void EnterRule(string ruleName)
		{
			Transmit("enterRule " + ruleName);
		}

		public override void EnterAlt(int alt)
		{
			Transmit("enterAlt " + alt);
		}

		public override void ExitRule(string ruleName)
		{
			Transmit("exitRule " + ruleName);
		}

		public override void EnterSubRule(int decisionNumber)
		{
			Transmit("enterSubRule " + decisionNumber);
		}

		public override void ExitSubRule(int decisionNumber)
		{
			Transmit("exitSubRule " + decisionNumber);
		}

		public override void EnterDecision(int decisionNumber)
		{
			Transmit("enterDecision " + decisionNumber);
		}

		public override void ExitDecision(int decisionNumber)
		{
			Transmit("exitDecision " + decisionNumber);
		}

		public override void ConsumeToken(IToken t)
		{
			string buf = SerializeToken(t);
			Transmit("consumeToken " + buf);
		}

		public override void ConsumeHiddenToken(IToken t)
		{
			string buf = SerializeToken(t);
			Transmit("consumeHiddenToken " + buf);
		}

		public override void LT(int i, IToken t)
		{
			if (t != null)
				Transmit("LT " + i + " " + SerializeToken(t));
		}

		public override void Mark(int i)
		{
			Transmit("mark " + i);
		}

		public override void Rewind(int i)
		{
			Transmit("rewind " + i);
		}

		public override void Rewind()
		{
			Transmit("rewind");
		}

		public override void BeginBacktrack(int level)
		{
			Transmit("beginBacktrack " + level);
		}

		public override void EndBacktrack(int level, bool successful)
		{
			Transmit("endBacktrack " + level + " " + (successful ? true.ToString() : false.ToString()));
		}

		public override void Location(int line, int pos)
		{
			Transmit("location " + line + " " + pos);
		}

		public override void RecognitionException(RecognitionException e)
		{
			StringBuilder buf = new StringBuilder(50);
			buf.Append("exception ");
			buf.Append(e.GetType().FullName);
			// dump only the data common to all exceptions for now
			buf.Append(" ");
			buf.Append(e.Index);
			buf.Append(" ");
			buf.Append(e.Line);
			buf.Append(" ");
			buf.Append(e.CharPositionInLine);
			Transmit(buf.ToString());
		}

		public override void BeginResync()
		{
			Transmit("beginResync");
		}

		public override void EndResync()
		{
			Transmit("endResync");
		}

		public override void SemanticPredicate(bool result, string predicate)
		{
			StringBuilder buf = new StringBuilder(50);
			buf.Append("semanticPredicate ");
			buf.Append(result);
			SerializeText(buf, predicate);
			Transmit(buf.ToString());
		}


		#region A S T  P a r s i n g  E v e n t s

		public override void ConsumeNode(object t)
		{
			StringBuilder buf = new StringBuilder(50);
			buf.Append("consumeNode ");
			SerializeNode(buf, t);
			Transmit(buf.ToString());
		}

		public override void LT(int i, object t)
		{
			int ID = adaptor.GetUniqueID(t);
			string text = adaptor.GetNodeText(t);
			int type = adaptor.GetNodeType(t);
			StringBuilder buf = new StringBuilder(50);
			buf.Append("LN "); // lookahead node; distinguish from LT in protocol
			buf.Append(i);
			SerializeNode(buf, t);
			Transmit(buf.ToString());
		}

		#endregion


		#region A S T  E v e n t s

		public override void GetNilNode(object t)
		{
			int ID = adaptor.GetUniqueID(t);
			Transmit("nilNode " + ID);
		}

		public override void CreateNode(object t)
		{
			int ID = adaptor.GetUniqueID(t);
			string text = adaptor.GetNodeText(t);
			int type = adaptor.GetNodeType(t);
			StringBuilder buf = new StringBuilder(50);
			buf.Append("createNodeFromTokenElements ");
			buf.Append(ID);
			buf.Append(" ");
			buf.Append(type);
			SerializeText(buf, text);
			Transmit(buf.ToString());
		}

		public override void CreateNode(object node, IToken token)
		{
			int ID = adaptor.GetUniqueID(node);
			int tokenIndex = token.TokenIndex;
			Transmit("createNode " + ID + " " + tokenIndex);
		}

		public override void BecomeRoot(object newRoot, object oldRoot)
		{
			int newRootID = adaptor.GetUniqueID(newRoot);
			int oldRootID = adaptor.GetUniqueID(oldRoot);
			Transmit("becomeRoot " + newRootID + " " + oldRootID);
		}

		public override void AddChild(object root, object child)
		{
			int rootID = adaptor.GetUniqueID(root);
			int childID = adaptor.GetUniqueID(child);
			Transmit("addChild " + rootID + " " + childID);
		}

		public override void SetTokenBoundaries(object t, int tokenStartIndex, int tokenStopIndex)
		{
			int ID = adaptor.GetUniqueID(t);
			Transmit("setTokenBoundaries " + ID + " " + tokenStartIndex + " " + tokenStopIndex);
		}

		#endregion

		#region Support

		protected internal virtual string SerializeToken(IToken t)
		{
			StringBuilder buf = new StringBuilder(50);
			buf.Append(t.TokenIndex); buf.Append(' ');
			buf.Append(t.Type); buf.Append(' ');
			buf.Append(t.Channel); buf.Append(' ');
			buf.Append(t.Line); buf.Append(' ');
			buf.Append(t.CharPositionInLine);
			SerializeText(buf, t.Text);
			return buf.ToString();
		}
		
		protected internal virtual string EscapeNewlines(string txt)
		{
			txt = txt.Replace("%", "%25"); // escape all escape char ;)
			txt = txt.Replace("\n", "%0A"); // escape \n
			txt = txt.Replace("\r", "%0D"); // escape \r
			return txt;
		}

		protected internal void SerializeNode(StringBuilder buf, object t)
		{
			int ID = adaptor.GetUniqueID(t);
			string text = adaptor.GetNodeText(t);
			int type = adaptor.GetNodeType(t);
			buf.Append(" ");
			buf.Append(ID);
			buf.Append(" ");
			buf.Append(type);
			IToken token = adaptor.GetToken(t);
			int line = -1;
			int pos = -1;
			if (token != null)
			{
				line = token.Line;
				pos = token.CharPositionInLine;
			}
			buf.Append(" ");
			buf.Append(line);
			buf.Append(" ");
			buf.Append(pos);
			int tokenIndex = adaptor.GetTokenStartIndex(t);
			buf.Append(" ");
			buf.Append(tokenIndex);
			SerializeText(buf, text);
		}

		protected void SerializeText(StringBuilder buf, string text)
		{
			buf.Append(" \"");
			if (text == null)
			{
				text = "";
			}
			// escape \n and \r all text for token appears to exist on one line
			// this escape is slow but easy to understand
			text = EscapeNewlines(text);
			buf.Append(text);
		}

		#endregion
	}
}