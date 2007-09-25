namespace antlr.debug
{
    using System;

    public abstract class InputBufferListenerBase : InputBufferListener, Listener
    {
        protected InputBufferListenerBase()
        {
        }

        public virtual void doneParsing(object source, TraceEventArgs e)
        {
        }

        public virtual void inputBufferConsume(object source, InputBufferEventArgs e)
        {
        }

        public virtual void inputBufferLA(object source, InputBufferEventArgs e)
        {
        }

        public virtual void inputBufferMark(object source, InputBufferEventArgs e)
        {
        }

        public virtual void inputBufferRewind(object source, InputBufferEventArgs e)
        {
        }

        public virtual void refresh()
        {
        }
    }
}

