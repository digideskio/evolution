/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include "camel-mime-message.h"
#include "camel-mime-body-part.h"
#include "camel-multipart.h"
#include "stdio.h"
#include "camel-stream.h"
#include "camel-stream-fs.h"
#include "camel-log.h"
#include "camel.h"

int
main (int argc, char**argv)
{
	CamelMimeMessage *message;
	CamelMultipart *multipart;
	CamelMimeBodyPart *body_part;
	
	/*  FILE *output_file; */
	CamelStream *stream;

	gtk_init (&argc, &argv);
	camel_init ();
	message = camel_mime_message_new_with_session( (CamelSession *)NULL);
	camel_mime_part_set_description (CAMEL_MIME_PART (message), "a test");
	camel_mime_part_add_header (CAMEL_MIME_PART (message), "X-test1", "the value of a test");
	camel_mime_part_add_header (CAMEL_MIME_PART (message), "X-test2", "the value of another test");
	/*camel_mime_part_add_content_language (CAMEL_MIME_PART (message), g_string_new ("es-ca"));*/

	camel_mime_message_set_received_date (message, "Thu, 20 May 1999, 10:39:14 +0200");
	camel_mime_message_set_subject (message, "A test message");
	camel_mime_message_set_reply_to (message, "toto@toto.com");
	camel_mime_message_set_from (message, "Bertrand.Guiheneuf@inria.fr");

	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_TO, "franck.dechamps@alseve.fr");
	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_TO, "mc@alseve.fr");
	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_TO, "richard.lengagne@inria.fr");
	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_CC, "Francois.fleuret@inria.fr");
	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_CC, "maury@justmagic.com");
 	camel_mime_message_add_recipient (message, RECIPIENT_TYPE_BCC, "Bertrand.Guiheneuf@aful.org");

	
	multipart = camel_multipart_new ();
	body_part = camel_mime_body_part_new ();
	camel_mime_part_set_text (CAMEL_MIME_PART (body_part), "This is a test.\nThis is only a test.\n");
	camel_multipart_add_part (multipart, body_part);
	camel_mime_part_set_content_object (CAMEL_MIME_PART (message), CAMEL_DATA_WRAPPER (multipart));

	stream = camel_stream_fs_new_with_name ("mail1.test", CAMEL_STREAM_FS_WRITE );
	if (!stream)  {
		CAMEL_LOG_FULL_DEBUG ("could not open output file");
		exit(2);
	}
		       
	camel_data_wrapper_write_to_stream (CAMEL_DATA_WRAPPER (message), stream);
	camel_stream_close (stream);
	
	printf ("Test1 finished\n");
}
 
