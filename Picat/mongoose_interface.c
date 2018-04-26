#include "mongoose/mongoose.h"
#include "picat.h"


//
// BASIC SERVER
//
static const char *s_http_port = "8000";

static void serve_handler(struct mg_connection *c, int ev, void *p) {
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;

		// We have received an HTTP request. Parsed request is contained in `hm`.
		// Send HTTP reply to the client which shows full original request.
		mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
		mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
	}
}

mongoose_test(void) {
	struct mg_mgr mgr;
	struct mg_connection *c;

	mg_mgr_init(&mgr, NULL);
	c = mg_bind(&mgr, s_http_port, serve_handler);
	mg_set_protocol_http_websocket(c);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return PICAT_TRUE;
}


//
// BASIC CLIENT
//
char* s_url;
static int s_exit_flag = 0;
static TERM temp_term;

static void client_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	int connect_status;
	switch (ev) {
	case MG_EV_CONNECT:
		connect_status = *(int *)ev_data;
		if (connect_status != 0) {
			printf("Error connecting to %s: %s\n", s_url, strerror(connect_status));
			s_exit_flag = 1;
		}
		break;
	case MG_EV_HTTP_REPLY: {
		//printf("Got reply:\n%.*s\n", (int)hm->body.len, hm->body.p);

		TERM t = picat_build_list(), car, cdr;
		car = picat_get_car(t);
		cdr = picat_get_cdr(t);

		int len = hm->body.len;
		for (int i = 0; i < len - 1; i++) {
			char c[] = { hm->body.p[i],"\0" };
			picat_unify(car, picat_build_atom(c));
			TERM temp = picat_build_list();
			picat_unify(cdr, temp);

			car = picat_get_car(temp);
			cdr = picat_get_cdr(temp);
		}
		char c[] = { hm->body.p[len - 1],"\0" };
		picat_unify(car, picat_build_atom(c));
		picat_unify(cdr, picat_build_nil());

		temp_term = t;

		nc->flags |= MG_F_SEND_AND_CLOSE;
		s_exit_flag = 1;
		break;
	}
	default:
		break;
	}
}

load_http() {
	TERM ret, value;

	ret = picat_get_call_arg(1, 2);

	if (picat_is_var(ret)) {
		value = picat_get_call_arg(2, 2);
		if (picat_is_string(value)) {

			//to c string
			int mult = 1;
			s_url = calloc(512, sizeof(char));

			TERM tList = value;
			TERM x = picat_get_car(tList);
			int i = 0;
			while (x != picat_build_integer(0)) {
				i++;
				if (i > 512 * mult) {
					mult++;
					realloc(s_url, 512 * mult);
				}
				strcat(s_url, picat_get_atom_name(x));
				tList = picat_get_cdr(tList);
				x = picat_get_car(tList);
			}

			//setup connection
			struct mg_mgr mgr;

			mg_mgr_init(&mgr, NULL);
			mg_connect_http(&mgr, client_handler, s_url, NULL, NULL);

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

mongoose_cpreds() {
	insert_cpred("mongoose_test", 0, mongoose_test);
	insert_cpred("load_http", 2, load_http);

}