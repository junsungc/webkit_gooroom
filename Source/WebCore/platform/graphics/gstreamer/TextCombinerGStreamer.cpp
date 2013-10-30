/*
 * Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK) && defined(GST_API_VERSION_1)

#include "TextCombinerGStreamer.h"

static GstStaticPadTemplate sinkTemplate =
    GST_STATIC_PAD_TEMPLATE("sink_%u", GST_PAD_SINK, GST_PAD_REQUEST,
        GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate srcTemplate =
    GST_STATIC_PAD_TEMPLATE("src", GST_PAD_SRC, GST_PAD_ALWAYS,
        GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC(webkitTextCombinerDebug);
#define GST_CAT_DEFAULT webkitTextCombinerDebug

#define webkit_text_combiner_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE(WebKitTextCombiner, webkit_text_combiner, GST_TYPE_BIN,
    GST_DEBUG_CATEGORY_INIT(webkitTextCombinerDebug, "webkittextcombiner", 0,
        "webkit text combiner"));


static void webkit_text_combiner_init(WebKitTextCombiner* combiner)
{
    combiner->funnel = gst_element_factory_make("funnel", NULL);
    ASSERT(combiner->funnel);

    gboolean ret = gst_bin_add(GST_BIN(combiner), combiner->funnel);
    UNUSED_PARAM(ret);
    ASSERT(ret);

    GstPad* pad = gst_element_get_static_pad(combiner->funnel, "src");
    ASSERT(pad);

    ret = gst_element_add_pad(GST_ELEMENT(combiner), gst_ghost_pad_new("src", pad));
    ASSERT(ret);
}

static GstPadProbeReturn webkitTextCombinerPadEvent(GstPad* pad, GstPadProbeInfo *info, gpointer)
{
    gboolean ret;
    UNUSED_PARAM(ret);
    WebKitTextCombiner* combiner = WEBKIT_TEXT_COMBINER(gst_pad_get_parent(pad));
    ASSERT(combiner);

    GstEvent* event = gst_pad_probe_info_get_event(info);
    ASSERT(event);

    if (GST_EVENT_TYPE(event) == GST_EVENT_CAPS) {
        GstCaps* caps;
        gst_event_parse_caps(event, &caps);
        ASSERT(caps);

        GstPad* target = gst_ghost_pad_get_target(GST_GHOST_PAD(pad));
        ASSERT(target);

        GstElement* targetParent = gst_pad_get_parent_element(target);
        ASSERT(targetParent);

        GstCaps* textCaps = gst_caps_new_empty_simple("text/x-raw");
        if (gst_caps_can_intersect(textCaps, caps)) {
            /* Caps are plain text, put a WebVTT encoder between the ghostpad and
             * the funnel */
            if (targetParent == combiner->funnel) {
                /* Setup a WebVTT encoder */
                GstElement* encoder = gst_element_factory_make("webvttenc", NULL);
                ASSERT(encoder);

                ret = gst_bin_add(GST_BIN(combiner), encoder);
                ASSERT(ret);

                ret = gst_element_sync_state_with_parent(encoder);
                ASSERT(ret);

                /* Switch the ghostpad to target the WebVTT encoder */
                GstPad* sinkPad = gst_element_get_static_pad(encoder, "sink");
                ASSERT(sinkPad);

                ret = gst_ghost_pad_set_target(GST_GHOST_PAD(pad), sinkPad);
                ASSERT(ret);
                gst_object_unref(sinkPad);

                /* Connect the WebVTT encoder to the funnel */
                GstPad* srcPad = gst_element_get_static_pad(encoder, "src");
                ASSERT(srcPad);

                ret = GST_PAD_LINK_SUCCESSFUL(gst_pad_link(srcPad, target));
                ASSERT(ret);
                gst_object_unref(srcPad);
            } /* else: pipeline is already correct */
        } else {
            /* Caps are not plain text, remove the WebVTT encoder */
            if (targetParent != combiner->funnel) {
                /* Get the funnel sink pad */
                GstPad* srcPad = gst_element_get_static_pad(targetParent, "src");
                ASSERT(srcPad);

                GstPad* sinkPad = gst_pad_get_peer(srcPad);
                ASSERT(sinkPad);
                gst_object_unref(srcPad);

                /* Switch the ghostpad to target the funnel */
                ret = gst_ghost_pad_set_target(GST_GHOST_PAD(pad), sinkPad);
                ASSERT(ret);
                gst_object_unref(sinkPad);

                /* Remove the WebVTT encoder */
                ret = gst_bin_remove(GST_BIN(combiner), targetParent);
                ASSERT(ret);
            } /* else: pipeline is already correct */
        }
        gst_caps_unref(textCaps);
        gst_object_unref(targetParent);
        gst_object_unref(target);
    }
    gst_object_unref(combiner);

    return GST_PAD_PROBE_OK;
}

static GstPad* webkitTextCombinerRequestNewPad(GstElement * element,
    GstPadTemplate * templ, const gchar * name, const GstCaps * caps)
{
    gboolean ret;
    UNUSED_PARAM(ret);
    ASSERT(templ);

    WebKitTextCombiner* combiner = WEBKIT_TEXT_COMBINER(element);
    ASSERT(combiner);

    GstPad* pad = gst_element_request_pad(combiner->funnel, templ, name, caps);
    ASSERT(pad);

    GstPad* ghostPad = gst_ghost_pad_new(NULL, pad);
    ASSERT(ghostPad);

    gst_pad_add_probe(ghostPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, webkitTextCombinerPadEvent, NULL, NULL);

    ret = gst_pad_set_active(ghostPad, true);
    ASSERT(ret);

    ret = gst_element_add_pad(GST_ELEMENT(combiner), ghostPad);
    ASSERT(ret);
    return ghostPad;
}

static void webkitTextCombinerReleasePad(GstElement *element, GstPad *pad)
{
    WebKitTextCombiner* combiner = WEBKIT_TEXT_COMBINER(element);
    GstPad* peer = gst_pad_get_peer(pad);
    if (peer) {
        GstElement* parent = gst_pad_get_parent_element(peer);
        ASSERT(parent);
        gst_element_release_request_pad(parent, peer);
        if (parent != combiner->funnel)
            gst_bin_remove(GST_BIN(combiner), parent);
    }

    gst_element_remove_pad(element, pad);
}

static void webkit_text_combiner_class_init(WebKitTextCombinerClass* klass)
{
    GstElementClass* elementClass = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(elementClass, gst_static_pad_template_get(&sinkTemplate));
    gst_element_class_add_pad_template(elementClass, gst_static_pad_template_get(&srcTemplate));

    gst_element_class_set_metadata(elementClass, "WebKit text combiner", "Generic",
        "A funnel that accepts any caps, but converts plain text to WebVTT",
        "Brendan Long <b.long@cablelabs.com>");

    elementClass->request_new_pad =
        GST_DEBUG_FUNCPTR(webkitTextCombinerRequestNewPad);
    elementClass->release_pad =
        GST_DEBUG_FUNCPTR(webkitTextCombinerReleasePad);
}

GstElement* webkitTextCombinerNew()
{
    return GST_ELEMENT(g_object_new(WEBKIT_TYPE_TEXT_COMBINER, 0));
}

#endif // ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK) && defined(GST_API_VERSION_1)
