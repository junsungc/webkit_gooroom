/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WorkerThreadableWebSocketChannel_h
#define WorkerThreadableWebSocketChannel_h

#if ENABLE(WEB_SOCKETS)

#include "ThreadableWebSocketChannel.h"
#include "WebSocketChannelClient.h"
#include "WorkerGlobalScope.h"

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>
#include <wtf/Threading.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class URL;
class ScriptExecutionContext;
class ThreadableWebSocketChannelClientWrapper;
class WorkerGlobalScope;
class WorkerLoaderProxy;
class WorkerRunLoop;

class WorkerThreadableWebSocketChannel : public RefCounted<WorkerThreadableWebSocketChannel>, public ThreadableWebSocketChannel {
    WTF_MAKE_FAST_ALLOCATED;
public:
    static PassRefPtr<ThreadableWebSocketChannel> create(WorkerGlobalScope* workerGlobalScope, WebSocketChannelClient* client, const String& taskMode)
    {
        return adoptRef(new WorkerThreadableWebSocketChannel(workerGlobalScope, client, taskMode));
    }
    virtual ~WorkerThreadableWebSocketChannel();

    // ThreadableWebSocketChannel functions.
    virtual void connect(const URL&, const String& protocol) override;
    virtual String subprotocol() override;
    virtual String extensions() override;
    virtual ThreadableWebSocketChannel::SendResult send(const String& message) override;
    virtual ThreadableWebSocketChannel::SendResult send(const JSC::ArrayBuffer&, unsigned byteOffset, unsigned byteLength) override;
    virtual ThreadableWebSocketChannel::SendResult send(Blob&) override;
    virtual unsigned long bufferedAmount() const override;
    virtual void close(int code, const String& reason) override;
    virtual void fail(const String& reason) override;
    virtual void disconnect() override; // Will suppress didClose().
    virtual void suspend() override;
    virtual void resume() override;

    // Generated by the bridge.  The Peer and its bridge should have identical
    // lifetimes.
    class Peer : public WebSocketChannelClient {
        WTF_MAKE_NONCOPYABLE(Peer); WTF_MAKE_FAST_ALLOCATED;
    public:
        static Peer* create(PassRefPtr<ThreadableWebSocketChannelClientWrapper> clientWrapper, WorkerLoaderProxy& loaderProxy, ScriptExecutionContext* context, const String& taskMode)
        {
            return new Peer(clientWrapper, loaderProxy, context, taskMode);
        }
        ~Peer();

        void connect(const URL&, const String& protocol);
        void send(const String& message);
        void send(const JSC::ArrayBuffer&);
        void send(Blob&);
        void bufferedAmount();
        void close(int code, const String& reason);
        void fail(const String& reason);
        void disconnect();
        void suspend();
        void resume();

        // WebSocketChannelClient functions.
        virtual void didConnect() override;
        virtual void didReceiveMessage(const String& message) override;
        virtual void didReceiveBinaryData(PassOwnPtr<Vector<char>>) override;
        virtual void didUpdateBufferedAmount(unsigned long bufferedAmount) override;
        virtual void didStartClosingHandshake() override;
        virtual void didClose(unsigned long unhandledBufferedAmount, ClosingHandshakeCompletionStatus, unsigned short code, const String& reason) override;
        virtual void didReceiveMessageError() override;

    private:
        Peer(PassRefPtr<ThreadableWebSocketChannelClientWrapper>, WorkerLoaderProxy&, ScriptExecutionContext*, const String& taskMode);

        RefPtr<ThreadableWebSocketChannelClientWrapper> m_workerClientWrapper;
        WorkerLoaderProxy& m_loaderProxy;
        RefPtr<ThreadableWebSocketChannel> m_mainWebSocketChannel;
        String m_taskMode;
    };

    using RefCounted<WorkerThreadableWebSocketChannel>::ref;
    using RefCounted<WorkerThreadableWebSocketChannel>::deref;

protected:
    virtual void refThreadableWebSocketChannel() { ref(); }
    virtual void derefThreadableWebSocketChannel() { deref(); }

private:
    // Bridge for Peer.  Running on the worker thread.
    class Bridge : public RefCounted<Bridge> {
    public:
        static PassRefPtr<Bridge> create(PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, PassRefPtr<WorkerGlobalScope> workerGlobalScope, const String& taskMode)
        {
            return adoptRef(new Bridge(workerClientWrapper, workerGlobalScope, taskMode));
        }
        ~Bridge();
        void initialize();
        void connect(const URL&, const String& protocol);
        ThreadableWebSocketChannel::SendResult send(const String& message);
        ThreadableWebSocketChannel::SendResult send(const JSC::ArrayBuffer&, unsigned byteOffset, unsigned byteLength);
        ThreadableWebSocketChannel::SendResult send(Blob&);
        unsigned long bufferedAmount();
        void close(int code, const String& reason);
        void fail(const String& reason);
        void disconnect();
        void suspend();
        void resume();

        using RefCounted<Bridge>::ref;
        using RefCounted<Bridge>::deref;

    private:
        Bridge(PassRefPtr<ThreadableWebSocketChannelClientWrapper>, PassRefPtr<WorkerGlobalScope>, const String& taskMode);

        static void setWebSocketChannel(ScriptExecutionContext*, Bridge* thisPtr, Peer*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>);

        // Executed on the main thread to create a Peer for this bridge.
        static void mainThreadInitialize(ScriptExecutionContext&, WorkerLoaderProxy*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>, const String& taskMode);

        // Executed on the worker context's thread.
        void clearClientWrapper();

        void setMethodNotCompleted();
        void waitForMethodCompletion();

        RefPtr<ThreadableWebSocketChannelClientWrapper> m_workerClientWrapper;
        RefPtr<WorkerGlobalScope> m_workerGlobalScope;
        WorkerLoaderProxy& m_loaderProxy;
        String m_taskMode;
        Peer* m_peer;
    };

    WorkerThreadableWebSocketChannel(WorkerGlobalScope*, WebSocketChannelClient*, const String& taskMode);

    static void mainThreadConnect(ScriptExecutionContext&, Peer*, const URL&, const String& protocol);
    static void mainThreadSend(ScriptExecutionContext&, Peer*, const String& message);
    static void mainThreadSendArrayBuffer(ScriptExecutionContext&, Peer*, PassOwnPtr<Vector<char>>);
    static void mainThreadSendBlob(ScriptExecutionContext&, Peer*, const URL&, const String& type, long long size);
    static void mainThreadBufferedAmount(ScriptExecutionContext&, Peer*);
    static void mainThreadClose(ScriptExecutionContext&, Peer*, int code, const String& reason);
    static void mainThreadFail(ScriptExecutionContext&, Peer*, const String& reason);
    static void mainThreadDestroy(ScriptExecutionContext&, Peer*);
    static void mainThreadSuspend(ScriptExecutionContext&, Peer*);
    static void mainThreadResume(ScriptExecutionContext&, Peer*);

    class WorkerGlobalScopeDidInitializeTask;

    RefPtr<WorkerGlobalScope> m_workerGlobalScope;
    RefPtr<ThreadableWebSocketChannelClientWrapper> m_workerClientWrapper;
    RefPtr<Bridge> m_bridge;
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // WorkerThreadableWebSocketChannel_h
