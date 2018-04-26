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
static const char *s_url = "http://people.sc.fsu.edu/~jburkardt/data/csv/deniro.csv";
static int s_exit_flag = 0;

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
	case MG_EV_HTTP_REPLY:
		printf("Got reply:\n%.*s\n", (int)hm->body.len, hm->body.p);
		nc->flags |= MG_F_SEND_AND_CLOSE;
		s_exit_flag = 1;
		break;
	default:
		break;
	}
}

mongoose_test_2(void) {
	struct mg_mgr mgr;

	mg_mgr_init(&mgr, NULL);
	mg_connect_http(&mgr, client_handler, s_url, NULL, NULL);


	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return PICAT_TRUE;
}


mongoose_cpreds() {
	insert_cpred("mongoose_test", 0, mongoose_test);
	insert_cpred("mongoose_test_2", 0, mongoose_test_2);

}