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
			else
			{
				picat_unify(name_cdr, picat_build_nil());
				picat_unify(value_cdr, picat_build_nil());
			}
		}
	}

	add_to_picat_map(ret, cstring_to_picat("header_names", 12), header_names);
	add_to_picat_map(ret, cstring_to_picat("header_values", 13), header_values);

	return ret;
}

struct http_message* picat_to_http_message(TERM t) {
	struct http_message *hm = calloc(1, sizeof(struct http_message));

	char* message = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("message", 7)));
	char* body = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("body", 4)));

	char* method = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("method", 6)));
	char* uri = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("uri", 3)));
	char* proto = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("proto", 5)));

	int resp_code = picat_get_integer(picat_map_get(t, cstring_to_picat("resp_code", 9)));
	char* resp_status_msg = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("resp_status_msg", 15)));

	char* query_string = picat_string_to_cstring(picat_map_get(t, cstring_to_picat("query_string", 12)));

	hm->message = mg_mk_str(message);
	hm->body = mg_mk_str(body);

	hm->method = mg_mk_str(method);
	hm->uri = mg_mk_str(uri);
	hm->proto = mg_mk_str(proto);

	hm->resp_code = resp_code;
	hm->resp_status_msg = mg_mk_str(resp_status_msg);

	hm->query_string = mg_mk_str(query_string);

	int i = 0;

	TERM n_car, n_cdr, v_car, v_cdr, h_names, h_values;

	h_names = picat_map_get(t, cstring_to_picat("header_names", 12));
	h_values = picat_map_get(t, cstring_to_picat("header_values", 13));

	n_car = picat_get_car(h_names);
	n_cdr = picat_get_cdr(h_names);

	v_car = picat_get_car(h_values);
	v_cdr = picat_get_cdr(h_values);

	while (n_car != n_cdr) {
		char* name = picat_string_to_cstring(n_car);
		hm->header_names[i] = mg_mk_str(name);

		char* value = picat_string_to_cstring(v_car);
		hm->header_values[i] = mg_mk_str(value);

		TERM n_temp = n_cdr;
		n_car = picat_get_car(n_temp);
		n_cdr = picat_get_cdr(n_temp);

		TERM v_temp = v_cdr;
		v_car = picat_get_car(v_temp);
		v_cdr = picat_get_cdr(v_temp);

		i++;
	}

	return hm;
}

struct http_message* empty_message() {
	struct http_message *hm = calloc(1,sizeof(struct http_message));
	
	struct mg_str blank = mg_mk_str("");
	hm->message = blank;
	hm->body = blank;

	hm->method = blank;
	hm->uri = blank;
	hm->proto = blank;

	hm->resp_code = 0;

	hm->resp_status_msg = blank;
	hm->query_string = blank;

	for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
		hm->header_names[i] = blank;
		
		hm->header_values[i] = blank;
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
struct mg_mgr* s_mgr;
struct mg_connection** connections;
struct mg_connection* s_cn;

struct http_message* requests;
int* events;

int requests_index;

#define MAX_REQUESTS 100

static void server_handler(struct mg_connection *nc, int ev, void *p) {
	if (ev == 100 || ev == 101 || ev == 102) {
		struct http_message* hm = (struct http_message*) p;

		if (requests_index < MAX_REQUESTS) {
			requests[requests_index] = *((struct http_message*) p);
			connections[requests_index] = nc;
			events[requests_index] = ev;

			requests_index++;
		}
	}	
}

http_serve() {
	TERM request = picat_get_call_arg(1, 1);

	if (b_IS_MAP_c(request)) {
		TERM message = picat_map_get(request, cstring_to_picat("message", 7));

		struct http_message* hm = picat_to_http_message(message);
		int connection = picat_get_integer(picat_map_get(request, cstring_to_picat("connection", 10)));
		struct mg_connection* cn = connections[connection];

		mg_serve_http(cn, hm, s_http_server_opts);

		return PICAT_TRUE;
	}
	return PICAT_FALSE;

}

http_serve_file() {
	TERM request, message, path, mime, headers;

	request = picat_get_call_arg(1, 4);

	if (b_IS_MAP_c(request)) {
		message = picat_map_get(request, cstring_to_picat("message", 7));


		struct http_message* hm = picat_to_http_message(message);
		int connection = picat_get_integer(picat_map_get(request, cstring_to_picat("connection", 10)));
		struct mg_connection* cn = connections[connection];

		path = picat_get_call_arg(2, 4);
		mime = picat_get_call_arg(3, 4);
		headers = picat_get_call_arg(4, 4);

		if (picat_is_string(path) && picat_is_string(mime) && picat_is_string(headers))  {
			mg_http_serve_file(cn, hm, picat_string_to_cstring(path), mg_mk_str(picat_string_to_cstring(mime)), mg_mk_str(picat_string_to_cstring(headers)));

			return PICAT_TRUE;
		}
	}
	return PICAT_FALSE;
}


http_send_redirect() {
	TERM request, status, location, headers;

	request = picat_get_call_arg(1, 4);

	if (b_IS_MAP_c(request)) {
		int connection = picat_get_integer(picat_map_get(request, cstring_to_picat("connection", 10)));
		struct mg_connection* cn = connections[connection];

		status = picat_get_call_arg(2, 4);

		if (picat_is_integer(status)) {
			int stat = picat_get_integer(status);

			if (stat == 302 || stat == 301) {
				location = picat_get_call_arg(3, 4);
				headers = picat_get_call_arg(4, 4);

				if (picat_is_string(location) && picat_is_string(headers)) {
					char* loc, *head;
					loc = picat_string_to_cstring(location);
					head = picat_string_to_cstring(headers);

					if (strcmp(head, "") == 0)
						head = NULL;

					mg_http_send_redirect(cn, stat, mg_mk_str(loc), mg_mk_str(head));

						return PICAT_TRUE;
				}
				printf("Third and forth arguments must be strings.\n");
			}
			printf("Second argument must be 301 or 302.\n");
		}
		else {
			printf("Second argument must be an integer.\n");
		}
	}
	return PICAT_FALSE;
}


http_print() {
	TERM request, message, location, headers;

	request = picat_get_call_arg(1, 2);

	if (b_IS_MAP_c(request)) {
		message = picat_get_call_arg(2, 2);

		if (picat_is_string(message)) {
			char* msg = picat_string_to_cstring(message);
			int len = strlen(msg);

			int connection = picat_get_integer(picat_map_get(request, cstring_to_picat("connection", 10)));
			struct mg_connection* cn = connections[connection];

			mg_send_head(cn, 200, len, "Content-Type: text/plain");
			mg_printf(cn, msg);

			return PICAT_TRUE;
		}
	}

	return PICAT_FALSE;
}

http_server_poll() {
	if (s_mgr == NULL) {
		printf("Server needs to be started first.\n");
		return PICAT_TRUE;
	}
	TERM ret, time;
	ret = picat_get_call_arg(1, 2);

	if (picat_is_var(ret)) {
		time = picat_get_call_arg(2, 2);

		if (picat_is_integer(time)) {
			int timeout_ms = picat_get_integer(time);

			int i;

			if (s_mgr->num_ifaces == 0) {
				return PICAT_TRUE;
			}

			requests_index = 0;

			for (i = 0; i < s_mgr->num_ifaces; i++) {
				s_mgr->ifaces[i]->vtable->poll(s_mgr->ifaces[i], timeout_ms);
			}

			if (requests_index == 0) {
				picat_unify(ret, picat_build_nil());
			}
			else {
				TERM l = picat_build_list();
				TERM car = picat_get_car(l);
				TERM cdr = picat_get_cdr(l);

				for (i = 0; i < requests_index; i++) {
					TERM pair = new_picat_map(3);
					add_to_picat_map(pair, cstring_to_picat("event", 5), picat_build_integer(events[i]));
					add_to_picat_map(pair, cstring_to_picat("message", 7), http_message_to_Picat(&requests[i]));
					add_to_picat_map(pair, cstring_to_picat("connection", 10), picat_build_integer(i));

					picat_unify(car, pair);

					if (i < requests_index - 1) {
						TERM temp = picat_build_list();
						picat_unify(cdr, temp);

						car = picat_get_car(temp);
						cdr = picat_get_cdr(temp);
					}
					else {
						picat_unify(cdr, picat_build_nil());
					}
				}
				picat_unify(ret, l);
			}
			return PICAT_TRUE;
		}
		else {
			//printf("Second argument must be an integer.\n");
		}
	}
	else {
		//printf("First argument must be a variable.\n");
	}
	printf("what/n");
	return PICAT_FALSE;
}


http_server() {
	if (s_mgr != NULL) {
		printf("Only one instance of the server can run at a time.\n");
		return PICAT_TRUE;
	}

	requests = realloc(requests, MAX_REQUESTS * sizeof(struct http_message));
	events = realloc(events, MAX_REQUESTS * sizeof(int));
	connections = realloc(events, MAX_REQUESTS * sizeof(struct mg_connection));

	TERM port, dir;

	port = picat_get_call_arg(1, 2);

	if (picat_is_integer(port)) {
		char* s_http_port[64];
		sprintf(s_http_port, "%d", picat_get_integer(port));
		
		s_mgr = calloc(1, sizeof(struct mg_mgr));

		mg_mgr_init(s_mgr, NULL);
		printf("Starting web server on port %s\n", s_http_port);
		s_cn = mg_bind(s_mgr, s_http_port, server_handler);
		
		if (s_cn == NULL) {
			printf("Failed to create listener\n");
			return 1;
		}

		// Set up HTTP server parameters
		mg_set_protocol_http_websocket(s_cn);

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
	}
	return PICAT_TRUE;
}

http_server_close() {
	if (s_mgr == NULL) {
		printf("No server is currently running.\n");
		return PICAT_TRUE;
	}

	mg_mgr_free(s_mgr);

	free(s_mgr);
	free(connections);
	free(s_cn);
	free(requests);
	free(events);

	s_mgr = connections = s_cn = requests = events = NULL;

	return PICAT_TRUE;
}



http_cpreds() {
	insert_cpred("http_request", 4, http_request);
	insert_cpred("http_server", 2, http_server);
	insert_cpred("http_server_close", 0, http_server_close);

	insert_cpred("http_server_poll", 2, http_server_poll);
	insert_cpred("http_serve", 1, http_serve);
	insert_cpred("http_serve_file", 4, http_serve_file);
	insert_cpred("http_send_redirect", 4, http_send_redirect);
	insert_cpred("http_print", 2, http_print);

}