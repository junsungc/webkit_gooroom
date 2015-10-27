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

WebInspector.AccessContentView = class AccessContentView extends WebInspector.ContentView
{
    constructor(representedObject)
    {
        super(representedObject);

        this.element.classList.add("access");

        var columns = {};
        // FIXME : Hardcoded Korean title's need to be corrected by creating kr.lproj in Localizations.
        columns.pid = {title: WebInspector.UIString("PID"), sortable: true};
        columns.url = {title: WebInspector.UIString("URL"), sortable: true};
        columns.printer = {title: "프린터", sortable: true};
        columns.usb = {title: WebInspector.UIString("USB"), sortable: true};
        columns.mic = {title: "마이크", sortable: true};
        columns.speaker = {title: "스피커", sortable: true};

        this._dataGrid = new WebInspector.DataGrid(columns, null, null);
        this._dataGrid.sortOrder = WebInspector.DataGrid.SortOrder.Ascending;
        this._dataGrid.sortColumnIdentifier = "pid";
        this._dataGrid.addEventListener(WebInspector.DataGrid.Event.SortChanged, this._sortDataGrid, this);

        this.element.appendChild(this._dataGrid.element);

        this._populate();
    }

    // Private

    _populate()
    {
        if (this.representedObject instanceof WebInspector.WebProcess) {
            var data = {type: "WebProcess", pid: this.representedObject.pid, url: this.representedObject.url, printer: this.representedObject.printer ? "O" : "X", usb: this.representedObject.usb ? "O" : "X", mic: this.representedObject.mic ? "O" : "X", speaker: this.representedObject.speaker ? "O" : "X" };
            var node = new WebInspector.DataGridNode(data, false);
            this._dataGrid.appendChild(node);
        } else if (this.representedObject instanceof WebInspector.UIProcess) {
            for (var webProcess of this.representedObject.webProcesses) {
                var data = {type: "WebProcess", pid: webProcess.pid, url: webProcess.url, printer: webProcess.printer ? "O" : "X", usb: webProcess.usb ? "O" : "X", mic: webProcess.mic ? "O" : "X", speaker: webProcess.speaker ? "O" : "X" };
                var node = new WebInspector.DataGridNode(data, false);
                this._dataGrid.appendChild(node);
            }
        }
    }

    _sortDataGrid()
    {
        var sortColumnIdentifier = this._dataGrid.sortColumnIdentifier || "pid";

        function comparator(a, b)
        {
            return a.data[sortColumnIdentifier].toString().localeCompare(b.data[sortColumnIdentifier].toString());
        }

        this._dataGrid.sortNodesImmediately(comparator);
    }

};
