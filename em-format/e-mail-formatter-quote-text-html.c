/*
 * e-mail-formatter-quote-text-html.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the program; if not, see <http://www.gnu.org/licenses/>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "e-mail-format-extensions.h"

#include <em-format/e-mail-formatter-extension.h>
#include <em-format/e-mail-formatter-quote.h>
#include <em-format/e-mail-stripsig-filter.h>
#include <em-format/e-mail-part-utils.h>
#include <e-util/e-util.h>

#include <glib/gi18n-lib.h>
#include <camel/camel.h>

#include <string.h>

typedef EMailFormatterExtension EMailFormatterQuoteTextHTML;
typedef EMailFormatterExtensionClass EMailFormatterQuoteTextHTMLClass;

G_DEFINE_TYPE (
	EMailFormatterQuoteTextHTML,
	e_mail_formatter_quote_text_html,
	E_TYPE_MAIL_FORMATTER_EXTENSION)

static const gchar *formatter_mime_types[] = {
	"text/html",
	NULL
};

static gboolean
emqfe_text_html_format (EMailFormatterExtension *extension,
                        EMailFormatter *formatter,
                        EMailFormatterContext *context,
                        EMailPart *part,
                        CamelStream *stream,
                        GCancellable *cancellable)
{
	EMailFormatterQuoteContext *qf_context;

	qf_context = (EMailFormatterQuoteContext *) context;

	camel_stream_write_string (
		stream, "\n<!-- text/html -->\n", cancellable, NULL);

	if ((qf_context->qf_flags & E_MAIL_FORMATTER_QUOTE_FLAG_KEEP_SIG) == 0) {
		CamelMimeFilter *sig_strip;
		CamelStream *filtered_stream;

		filtered_stream = camel_stream_filter_new (stream);

		sig_strip = e_mail_stripsig_filter_new (FALSE);
		camel_stream_filter_add (
			CAMEL_STREAM_FILTER (filtered_stream), sig_strip);
		g_object_unref (sig_strip);

		e_mail_formatter_format_text (
			formatter, part, filtered_stream, cancellable);
		camel_stream_flush (filtered_stream, cancellable, NULL);
		g_object_unref (filtered_stream);
	} else {
		e_mail_formatter_format_text (
			formatter, part, stream, cancellable);
	}

	return TRUE;
}

static const gchar *
emqfe_text_html_get_display_name (EMailFormatterExtension *extension)
{
	return _("HTML");
}

static const gchar *
emqfe_text_html_get_description (EMailFormatterExtension *extension)
{
	return _("Format part as HTML");
}

static void
e_mail_formatter_quote_text_html_class_init (EMailFormatterExtensionClass *class)
{
	class->mime_types = formatter_mime_types;
	class->format = emqfe_text_html_format;
	class->get_display_name = emqfe_text_html_get_display_name;
	class->get_description = emqfe_text_html_get_description;
}

static void
e_mail_formatter_quote_text_html_init (EMailFormatterExtension *extension)
{
}
