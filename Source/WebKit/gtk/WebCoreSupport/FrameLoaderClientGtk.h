/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FrameLoaderClientGtk_h
#define FrameLoaderClientGtk_h

#include "FrameLoaderClient.h"
#include "ProgressTrackerClient.h"
#include "ResourceResponse.h"
#include "webkitwebpolicydecision.h"

typedef struct _WebKitWebFrame WebKitWebFrame;

namespace WebCore {
class PluginView;
}

namespace WebKit {

    class FrameLoaderClient : public WebCore::FrameLoaderClient, public WebCore::ProgressTrackerClient {
    public:
        FrameLoaderClient(WebKitWebFrame* = 0);
        virtual ~FrameLoaderClient();
        virtual void frameLoaderDestroyed();

        void setWebFrame(WebKitWebFrame* frame) { m_frame = frame; }
        WebKitWebFrame*  webFrame() const { return m_frame; }

        virtual bool hasWebView() const;

        virtual void makeRepresentation(WebCore::DocumentLoader*) { }
        virtual void forceLayout();
        virtual void forceLayoutForNonHTML();

        virtual void setCopiesOnScroll();

        virtual void detachedFromParent2();
        virtual void detachedFromParent3();

        virtual void assignIdentifierToInitialRequest(unsigned long identifier, WebCore::DocumentLoader*, const WebCore::ResourceRequest&);

        virtual void dispatchWillSendRequest(WebCore::DocumentLoader*, unsigned long  identifier, WebCore::ResourceRequest&, const WebCore::ResourceResponse& redirectResponse);
        virtual bool shouldUseCredentialStorage(WebCore::DocumentLoader*, unsigned long identifier);
        virtual void dispatchDidReceiveAuthenticationChallenge(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::AuthenticationChallenge&);
        virtual void dispatchDidCancelAuthenticationChallenge(WebCore::DocumentLoader*, unsigned long  identifier, const WebCore::AuthenticationChallenge&);
        virtual void dispatchDidReceiveResponse(WebCore::DocumentLoader*, unsigned long  identifier, const WebCore::ResourceResponse&);
        virtual void dispatchDidReceiveContentLength(WebCore::DocumentLoader*, unsigned long identifier, int dataLength);
        virtual void dispatchDidFinishLoading(WebCore::DocumentLoader*, unsigned long  identifier);
        virtual void dispatchDidFailLoading(WebCore::DocumentLoader*, unsigned long  identifier, const WebCore::ResourceError&);
        virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int length);

        virtual void dispatchDidHandleOnloadEvents();
        virtual void dispatchDidReceiveServerRedirectForProvisionalLoad();
        virtual void dispatchDidCancelClientRedirect();
        virtual void dispatchWillPerformClientRedirect(const WebCore::URL&, double, double);
        virtual void dispatchDidChangeLocationWithinPage();
        virtual void dispatchDidNavigateWithinPage();
        virtual void dispatchDidPushStateWithinPage();
        virtual void dispatchDidReplaceStateWithinPage();
        virtual void dispatchDidPopStateWithinPage();
        virtual void dispatchWillClose();
        virtual void dispatchDidReceiveIcon();
        virtual void dispatchDidStartProvisionalLoad();
        virtual void dispatchDidReceiveTitle(const WebCore::StringWithDirection&);
        virtual void dispatchDidChangeIcons(WebCore::IconType);
        virtual void dispatchDidCommitLoad();
        virtual void dispatchDidFailProvisionalLoad(const WebCore::ResourceError&);
        virtual void dispatchDidFailLoad(const WebCore::ResourceError&);
        virtual void dispatchDidFinishDocumentLoad();
        virtual void dispatchDidFinishLoad();
        virtual void dispatchDidLayout(WebCore::LayoutMilestones);

        virtual WebCore::Frame* dispatchCreatePage(const WebCore::NavigationAction&);
        virtual void dispatchShow();

        virtual void dispatchDecidePolicyForResponse(const WebCore::ResourceResponse&, const WebCore::ResourceRequest&, WebCore::FramePolicyFunction);
        virtual void dispatchDecidePolicyForNewWindowAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, WTF::PassRefPtr<WebCore::FormState>, const WTF::String& frameName, WebCore::FramePolicyFunction);
        virtual void dispatchDecidePolicyForNavigationAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, WTF::PassRefPtr<WebCore::FormState>, WebCore::FramePolicyFunction);
        virtual void cancelPolicyCheck();

        virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&);

        virtual void dispatchWillSendSubmitEvent(WTF::PassRefPtr<WebCore::FormState>) { }
        virtual void dispatchWillSubmitForm(WTF::PassRefPtr<WebCore::FormState>, WebCore::FramePolicyFunction);

        virtual void revertToProvisionalState(WebCore::DocumentLoader*) { }
        virtual void setMainDocumentError(WebCore::DocumentLoader*, const WebCore::ResourceError&);

        virtual void progressStarted(WebCore::Frame&);
        virtual void progressEstimateChanged(WebCore::Frame&);
        virtual void progressFinished(WebCore::Frame&);

        virtual PassRefPtr<WebCore::Frame> createFrame(const WebCore::URL& url, const WTF::String& name, WebCore::HTMLFrameOwnerElement* ownerElement,
                                   const WTF::String& referrer, bool allowsScrolling, int marginWidth, int marginHeight);
        virtual PassRefPtr<WebCore::Widget> createPlugin(const WebCore::IntSize&, WebCore::HTMLPlugInElement*, const WebCore::URL&, const WTF::Vector<WTF::String>&, const WTF::Vector<WTF::String>&, const WTF::String&, bool);
        virtual void recreatePlugin(WebCore::Widget*) { }
        virtual void redirectDataToPlugin(WebCore::Widget* pluginWidget);
        virtual PassRefPtr<WebCore::Widget> createJavaAppletWidget(const WebCore::IntSize&, WebCore::HTMLAppletElement*, const WebCore::URL& baseURL, const WTF::Vector<WTF::String>& paramNames, const WTF::Vector<WTF::String>& paramValues);
        virtual WTF::String overrideMediaType() const;
        virtual void dispatchDidClearWindowObjectInWorld(WebCore::DOMWrapperWorld&);
        virtual void documentElementAvailable();
        virtual void didPerformFirstNavigation() const;

        virtual void registerForIconNotification(bool);

        virtual WebCore::ObjectContentType objectContentType(const WebCore::URL&, const WTF::String& mimeType, bool shouldPreferPlugInsForImages);

        virtual void setMainFrameDocumentReady(bool);

        virtual void startDownload(const WebCore::ResourceRequest&, const String& suggestedName = String());

        virtual void willChangeTitle(WebCore::DocumentLoader*);
        virtual void didChangeTitle(WebCore::DocumentLoader*);

        virtual void committedLoad(WebCore::DocumentLoader*, const char*, int);
        virtual void finishedLoading(WebCore::DocumentLoader*);

        virtual void updateGlobalHistory();
        virtual void updateGlobalHistoryRedirectLinks();
        virtual bool shouldGoToHistoryItem(WebCore::HistoryItem*) const;
        virtual bool shouldStopLoadingForHistoryItem(WebCore::HistoryItem*) const;

        virtual void didDisplayInsecureContent();
        virtual void didRunInsecureContent(WebCore::SecurityOrigin*, const WebCore::URL&);
        virtual void didDetectXSS(const WebCore::URL&, bool didBlockEntirePage);

        virtual WebCore::ResourceError cancelledError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError blockedError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError cannotShowURLError(const WebCore::ResourceRequest&);
        virtual WebCore::ResourceError interruptedForPolicyChangeError(const WebCore::ResourceRequest&);

        virtual WebCore::ResourceError cannotShowMIMETypeError(const WebCore::ResourceResponse&);
        virtual WebCore::ResourceError fileDoesNotExistError(const WebCore::ResourceResponse&);
        virtual WebCore::ResourceError pluginWillHandleLoadError(const WebCore::ResourceResponse&);

        virtual bool shouldFallBack(const WebCore::ResourceError&);

        virtual bool canHandleRequest(const WebCore::ResourceRequest&) const;
        virtual bool canShowMIMEType(const WTF::String&) const;
        virtual bool canShowMIMETypeAsHTML(const String& MIMEType) const;
        virtual bool representationExistsForURLScheme(const WTF::String&) const;
        virtual WTF::String generatedMIMETypeForURLScheme(const WTF::String&) const;

        virtual void frameLoadCompleted();
        virtual void saveViewStateToItem(WebCore::HistoryItem*);
        virtual void restoreViewState();
        virtual void provisionalLoadStarted();
        virtual void didFinishLoad();
        virtual void prepareForDataSourceReplacement();

        virtual WTF::PassRefPtr<WebCore::DocumentLoader> createDocumentLoader(const WebCore::ResourceRequest&, const WebCore::SubstituteData&);
        virtual void setTitle(const WebCore::StringWithDirection& title, const WebCore::URL&);

        virtual WTF::String userAgent(const WebCore::URL&);

        virtual void savePlatformDataToCachedFrame(WebCore::CachedFrame*);
        virtual void transitionToCommittedFromCachedFrame(WebCore::CachedFrame*);
        virtual void transitionToCommittedForNewPage();

        virtual void didSaveToPageCache();
        virtual void didRestoreFromPageCache();

        virtual void dispatchDidBecomeFrameset(bool);

        virtual bool canCachePage() const;
        virtual void convertMainResourceLoadToDownload(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&);

        virtual PassRefPtr<WebCore::FrameNetworkingContext> createNetworkingContext();

    private:
        WebKitWebFrame* m_frame;
        WebCore::ResourceResponse m_response;
        WebKitWebPolicyDecision* m_policyDecision;

        bool m_loadingErrorPage;

        // Plugin view to redirect data to
        WebCore::PluginView* m_pluginView;
        bool m_hasSentResponseToPlugin;

        virtual void dispatchDidCommitLoad(bool isNavigatingWithinPage);
    };

}

#endif
