/*
 * Copyright (C) 2015 Company 100, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

WebInspector.WorkerDetailsSection = class WorkerDetailsSection extends WebInspector.Object
{
    constructor(identifier, title)
    {
        super();

        console.assert(identifier);

        this._element = document.createElement("div");
        this._element.classList.add(identifier, "details-section");

        this._headerElement = document.createElement("div");
        this._headerElement.className = "worker-header";
        this._titleElement = document.createElement("span");

        this._contentElement = document.createElement("div");
        this._contentElement.className = "content";
        this._element.appendChild(this._contentElement);

        var columns = {};
        columns.url = {title: WebInspector.UIString("URL"), sortable: true};
        columns.permission = {title: WebInspector.UIString("Permission"), sortable: true};
        this._dataGrid = new WebInspector.DataGrid(columns, null, null);
        this._dataGrid.sortOrder = WebInspector.DataGrid.SortOrder.Ascending;
        this._dataGrid.sortColumnIdentifier = "url";
        this.element.appendChild(this._dataGrid.element);

        this._identifier = identifier;
        this.title = title;

        WebInspector.accessManager.addEventListener(WebInspector.AccessManager.Event.WorkersUpdated, this.refreshWorker.bind(this));
    }

    // Public

    refreshWorker()
    {
        this._dataGrid.removeChildren();
        for (var workerAccess of WebInspector.accessManager.workerAccesses) {
            var data = {url: workerAccess.scriptURL, permission: workerAccess.permission};
            var node = new WebInspector.DataGridNode(data, false);
            this._dataGrid.appendChild(node);
        }
    }

    get element()
    {
        return this._element;
    }

    get identifier()
    {
        return this._identifier;
    }

    get title()
    {
        return this._titleElement.textContent;
    }

    set title(title)
    {
        this._titleElement.textContent = title;
    }
};
