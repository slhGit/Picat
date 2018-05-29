#include "mongoose/mongoose.h"
#include "picat.h"
#include "picat_utilities.h"



TERM mg_str_to_picat(struct mg_str v) {
	return cstring_to_picat(v.p, v.len);
}

TERM http_message_to_Picat(struct http_message* v) {
	int C = 11;
	TERM ret = new_picat_map(C);

	add_to_picat_map(ret, cstring_to_picat("message", 7), mg_str_to_picat(v->message));
	add_to_picat_map(ret, cstring_to_picat("body", 4), mg_str_to_picat(v->body));

	add_to_picat_map(ret, cstring_to_picat("method", 6), mg_str_to_picat(v->method));
	add_to_picat_map(ret, cstring_to_picat("uri", 3), mg_str_to_picat(v->uri));
	add_to_picat_map(ret, cstring_to_picat("proto", 5), mg_str_to_picat(v->proto));

	add_to_picat_map(ret, cstring_to_picat("resp_code", 9), picat_build_integer(v->resp_code));
	add_to_picat_map(ret, cstring_to_picat("resp_status_msg", 15), mg_str_to_picat(v->resp_status_msg));

	add_to_picat_map(ret, cstring_to_picat("query_string", 12), mg_str_to_picat(v->query_string));

	TERM header_names = picat_build_list(), header_values = picat_build_list();
	TERM name_car = picat_get_car(header_names), name_cdr = picat_get_cdr(header_names);
	TERM value_car = picat_get_car(header_values), value_cdr = picat_get_cdr(header_values);

	int count = 0;
	for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
		if (v->header_names[i].len > 0) {
			count++;
		}
	}
	for (int i = 0; i < count; i++) {
		if (v->header_names[i].len > 0) {
			picat_unify(name_car, mg_str_to_picat(v->header_names[i]));
			picat_unify(value_car, mg_str_to_picat(v->header_values[i]));

			if (i < count - 1) {
				TERM n_temp = picat_build_list();
				picat_unify(name_cdr, n_temp);

				name_car = picat_get_car(n_temp);
				name_cdr = picat_get_cdr(n_temp);

				TERM v_temp = picat_build_list();
				picat_unify(value_cdr, v_temp);

				value_car = picat_get_car(v_temp);
				value_cdr = picat_get_cdr(v_temp);
			}
		}
	}

	add_to_picat_map(ret, cstring_to_picat("header_names", 12), header_names);
	add_to_picat_map(ret, cstring_to_picat("header_values", 13), header_values);

	return ret;
}

struct http_message* empty_message() {
	struct http_message *hm = calloc(1,sizeof(struct http_message));
	hm->message.p = "";
	hm->message.len = 0;

	hm->body.p = "";
	hm->body.len = 0;

	hm->method.p = "";
	hm->method.len = 0;

	hm->uri.p = "";
	hm->uri.len = 0;

	hm->proto.p = "";
	hm->proto.len = 0;

	hm->resp_code = 0;

	hm->resp_status_msg.p = "";
	hm->resp_status_msg.len = 0;

	hm->query_string.p = "";
	hm->query_string.len = 0;

	for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
		hm->header_names[i].p = "";
		hm->header_names[i].len = 0; 
		
		hm->header_values[i].p = "";
		hm->header_values[i].len = 0;
	}

	return hm;
}

//
// BASIC CLIENT
//
char* s_url;
static int s_exit_flag = 0;
static TERM temp_term = picat_build_nil;

static void client_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	int connect_status;
	switch (ev) {
		case MG_EV_CONNECT:
			connect_status = *(int *)ev_data;
			if (connect_status != 0) {
				printf("Cannot Connect to Server\n");
				struct http_message* temp = empty_message();
				temp_term = http_message_to_Picat(temp);
				free(temp);

				s_exit_flag = 1;
			}
			break;
		case MG_EV_HTTP_REPLY: {
			temp_term = http_message_to_Picat(hm);

			nc->flags |= MG_F_SEND_AND_CLOSE;
			s_exit_flag = 1;
			break;
		}
		default: {
			break;
		}
	}
}


http_request() {
	TERM ret, url, header, post;

	ret = picat_get_call_arg(1, 4);

	if (picat_is_var(ret)) {
		url = picat_get_call_arg(2, 4);
		header = picat_get_call_arg(3, 4);
		post = picat_get_call_arg(4, 4);

		if (picat_is_string(url)) {

			//to c string
			s_url = picat_string_to_cstring(url);

			//setup connection
			struct mg_mgr mgr;
			struct mg_connection* nc;
			
			mg_mgr_init(&mgr, NULL);
			char* s_headers = NULL, *s_post = NULL;

			if (picat_is_string(header))
				s_headers = picat_string_to_cstring(header);
			if (picat_is_string(post))
				s_post = picat_string_to_cstring(post);

			nc = mg_connect_http(&mgr, client_handler, s_url, s_headers, s_post);
			mg_set_protocol_http_websocket(nc);

			while (s_exit_flag == 0) {
				mg_mgr_poll(&mgr, 1000);
			}

			mg_mgr_free(&mgr);
			
			// cleanp
			free(s_url);
			s_exit_flag = 0;

			// return
			TERM t = temp_term;
			temp_term = picat_build_nil;

			return picat_unify(ret, t);
		}
		else {
			// this should be a string
			return PICAT_FALSE;
		}
	}
	else {
		// this should be a Var
		return PICAT_FALSE;
	}
}


//
// BASIC SERVER
//
static struct mg_serve_http_opts s_http_server_opts;

static void server_handler(struct mg_connection *nc, int ev, void *p) {
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message* m = (struct http_message *) p;

		printf("message:\n");
		printf("%s\n\n", m->message.p);

		printf("body:\n");
		printf("%s\n\n", m->body.p);

		printf("method:\n");
		printf("%s\n\n", m->method.p);

		printf("uri:\n");
		printf("%s\n\n", m->uri.p);

		printf("proto:\n");
		printf("%s\n\n", m->proto.p);

		printf("resp_code:\n");
		printf("%d\n\n", m->resp_code);

		printf("resp_status:\n");
		printf("%s\n\n", m->resp_status_msg.p);

		printf("query_string:\n");
		printf("%s\n\n", m->query_string.p);


		mg_serve_http(nc, m, s_http_server_opts);
	}
}

http_server() {
	TERM port, dir;

	port = picat_get_call_arg(1, 2);

	if (picat_is_integer(port)) {
		char* s_http_port[64];
		sprintf(s_http_port, "%d", picat_get_integer(port));
		struct mg_mgr mgr;
		struct mg_connection *nc;

		mg_mgr_init(&mgr, NULL);
		printf("Starting web server on port %s\n", s_http_port);
		nc = mg_bind(&mgr, s_http_port, server_handler);
		if (nc == NULL) {
			printf("Failed to create listener\n");
			return 1;
		}

		// Set up HTTP server parameters
		mg_set_protocol_http_websocket(nc);

		//set dir
		dir = picat_get_call_arg(2, 2);

		if (picat_is_string(dir)) {
			char* s_dir = picat_string_to_cstring(dir);
			s_http_server_opts.document_root = s_dir;
		}
		else {
			s_http_server_opts.document_root = ".";  // Serve current directory
		}

		s_http_server_opts.enable_directory_listing = "yes";

		char ch;
		for (;;) {
			mg_mgr_poll(&mgr, 1000);
		}
		mg_mgr_free(&mgr);

	}
	return PICAT_TRUE;
}



http_cpreds() {
	insert_cpred("http_request", 4, http_request);
	insert_cpred("http_server", 2, http_server);
}