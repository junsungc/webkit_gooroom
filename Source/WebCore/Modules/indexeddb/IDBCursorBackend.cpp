/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "IDBCursorBackend.h"

#if ENABLE(INDEXED_DATABASE)

#include "IDBCallbacks.h"
#include "IDBCursorBackendOperations.h"
#include "IDBDatabaseBackend.h"
#include "IDBDatabaseCallbacks.h"
#include "IDBDatabaseError.h"
#include "IDBDatabaseException.h"
#include "IDBKeyRange.h"
#include "IDBOperation.h"
#include "Logging.h"
#include "SharedBuffer.h"

namespace WebCore {

IDBCursorBackend::IDBCursorBackend(PassRefPtr<IDBBackingStoreCursorInterface> cursor, IndexedDB::CursorType cursorType, IDBDatabaseBackend::TaskType taskType, IDBTransactionBackend* transaction, int64_t objectStoreId)
    : m_taskType(taskType)
    , m_cursorType(cursorType)
    , m_database(&(transaction->database()))
    , m_transaction(transaction)
    , m_objectStoreId(objectStoreId)
    , m_cursor(cursor)
    , m_closed(false)
{
    m_transaction->registerOpenCursor(this);
}

IDBCursorBackend::~IDBCursorBackend()
{
    m_transaction->unregisterOpenCursor(this);
}


void IDBCursorBackend::continueFunction(PassRefPtr<IDBKey> key, PassRefPtr<IDBCallbacks> prpCallbacks, ExceptionCode&)
{
    LOG(StorageAPI, "IDBCursorBackend::continue");
    RefPtr<IDBCallbacks> callbacks = prpCallbacks;
    m_transaction->scheduleTask(m_taskType, CursorIterationOperation::create(this, key, callbacks));
}

void IDBCursorBackend::advance(unsigned long count, PassRefPtr<IDBCallbacks> prpCallbacks, ExceptionCode&)
{
    LOG(StorageAPI, "IDBCursorBackend::advance");
    RefPtr<IDBCallbacks> callbacks = prpCallbacks;
    m_transaction->scheduleTask(CursorAdvanceOperation::create(this, count, callbacks));
}

void IDBCursorBackend::deleteFunction(PassRefPtr<IDBCallbacks> prpCallbacks, ExceptionCode&)
{
    LOG(StorageAPI, "IDBCursorBackend::delete");
    ASSERT(m_transaction->mode() != IndexedDB::TransactionReadOnly);
    RefPtr<IDBKeyRange> keyRange = IDBKeyRange::create(m_cursor->primaryKey());
    m_database->deleteRange(m_transaction.get()->id(), m_objectStoreId, keyRange.release(), prpCallbacks);
}

void IDBCursorBackend::prefetchContinue(int numberToFetch, PassRefPtr<IDBCallbacks> prpCallbacks, ExceptionCode&)
{
    LOG(StorageAPI, "IDBCursorBackend::prefetchContinue");
    RefPtr<IDBCallbacks> callbacks = prpCallbacks;
    m_transaction->scheduleTask(m_taskType, CursorPrefetchIterationOperation::create(this, numberToFetch, callbacks));
}

void IDBCursorBackend::prefetchReset(int usedPrefetches, int)
{
    LOG(StorageAPI, "IDBCursorBackend::prefetchReset");
    m_cursor = m_savedCursor;
    m_savedCursor = 0;

    if (m_closed)
        return;
    if (m_cursor) {
        for (int i = 0; i < usedPrefetches; ++i) {
            bool ok = m_cursor->continueFunction();
            ASSERT_UNUSED(ok, ok);
        }
    }
}

void IDBCursorBackend::close()
{
    LOG(StorageAPI, "IDBCursorBackend::close");
    m_closed = true;
    m_cursor.clear();
    m_savedCursor.clear();
}

} // namespace WebCore

#endif // ENABLE(INDEXED_DATABASE)