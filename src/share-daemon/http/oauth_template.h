
/*
 *  Copyright 2016 Neo Natura 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 */  

#ifndef __OAUTH_TEMPLATE_H__
#define __OAUTH_TEMPLATE_H__

void oauth_response_token_template(shmap_t *sess, shbuf_t *buff, char *client_id);


void oauth_response_login_template(shmap_t *sess, shbuf_t *buff, char *client_id, char *warning);

void oauth_response_notfound_template(shbuf_t *buff);

void oauth_response_app_error_template(shmap_t *sess, shbuf_t *buff, char *client_id);

char *oauth_response_app_error_url(shmap_t *sess, char *client_id);

void oauth_response_2fa_template(shmap_t *sess, shbuf_t *buff, char *client_id);

char *oauth_app_header_html(shmap_t *sess, char *client_id);

void oauth_html_json_template(shbuf_t *buff, shjson_t *json);



#ifdef __OAUTH_TEMPLATE_C__

static const char *_oauth_response_access_html = 
  "<html>\r\n"
  "\r\n"
  "<body style=\"background-color : rgba(27,75,178,0.5); font-family : Helvetica,Arial,sans-serif,Georgia; padding : 0 0 0 0; color : #474a54; -webkit-font-smoothing : antialiased; font-size : 14px; line-height : 20px;\">\r\n"
  "\r\n"
  "<div style=\"margin-top : 8%%;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 600px; background-color : #000; color : #efefef; margin-top : 32px; box-shadow : 4px 4px 4px #1b1b1b; border-radius : 18px;\">\r\n"
  "\r\n"
  "<!-- NN Logo -->\r\n"
  "<div style=\"float : right; margin-right : 48px; margin-top : -24px;\">\r\n"
  "%s" /* neo natura logo */
  "</div>\r\n"
  "<!-- App Logo -->\r\n"
  "%s\r\n" /* header */
#if 0
  "<div style=\"clear : both;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; margin-left : 32px; padding : 16px 16px 16px 16px;\">\r\n"
  "<img src=\"%s\" style=\"max-width : 140px; max-height : 80px;\">\r\n"
  "</div>\r\n"
  "<div style=\"float : left; margin-left : 32px; font-size : 28px; height : 20px; margin-top : 75px;\">\r\n"
  "<span>%s</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
#endif
  "<div style=\"margin-top : 4px;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 500px; height : 0px; border : solid 1px #ccc;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; width : 128px; margin-left : 64px;\">%s</div>\r\n"
  "<div style=\"float : left; width : 340px; margin-top : 32px; margin-right : 32px;\"><span style=\"font-size : 24px; line-height : 26px;\">An application would like to connect to your account.</span></div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"font-size : 20px; width : 500px; margin-left : auto; margin-right : auto;\">\r\n"
  "<span>The app</span>\r\n"
  "<span style=\"font-weight : bold;\">%s</span>\r\n"
  "<span>would like the ability to perform actions on behalf of you.</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"margin-top : 32px;\"></div>\r\n"
  "\r\n"
  "<div style=\"font-size : 22px; text-align : center;\">\r\n"
  "<span>Allow</span>\r\n"
//  "<span style=\"font-weight : bold;\">Sample App</span>\r\n"
  "</span>access?</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "<div style=\"text-align : center; margin-left : auto; margin-right : auto; width : 250px;\">\r\n"
  "<div style=\"float : right; margin-left : 16px; margin-right : 16px; border : solid #efefef 1px; width : 80px; height : 32px; padding-top : 8px;\">\r\n"
  "<a href=\"/auth?client_id=%s&response_type=access&code=%s\" style=\"width : 100%; height : 100%; color : #efefef; text-decoration : none;\">Allow</a>\r\n"
  "</div>\r\n"
  "<div style=\"float : left; margin-left : 16px; margin-right : 16px; border : solid #efefef 1px; width : 80px; height : 32px; padding-top : 8px;\">\r\n"
  "<a href=\"%s\" style=\"width : 100%; height : 100%; color : #efefef; text-decoration : none;\">Deny</a>\r\n"
  "</div>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "\r\n"
  "<br>\r\n"
  "\r\n"
  "</div>\r\n"
  "\r\n"
  "\r\n"
  "</body>\r\n"
  "</html>\r\n";

static const char *_oauth_response_login_html =
  "<html>\r\n"
  "\r\n"
  "<body style=\"background-color : rgba(27,75,178,0.5); font-family : Helvetica,Arial,sans-serif,Georgia; padding : 0 0 0 0; color : #474a54; -webkit-font-smoothing : antialiased; font-size : 14px; line-height : 20px;\">\r\n"
  "\r\n"
  "<div style=\"margin-top : 8%%;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 600px; background-color : #000; color : #efefef; margin-top : 32px; box-shadow : 4px 4px 4px #1b1b1b; border-radius : 18px;\">\r\n"
  "\r\n"
  "<!-- NN Logo -->\r\n"
  "<div style=\"float : right; margin-right : 48px; margin-top : -24px;\">\r\n"
  "%s\r\n" /* neo natura logo */
  "</div>\r\n"
  "<!-- App Logo -->\r\n"
  "%s\r\n" /* header */
#if 0
  "<div style=\"clear : both;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; margin-left : 32px; padding : 16px 16px 16px 16px;\">\r\n"
  "<img src=\"%s\" style=\"max-width : 140px; max-height : 80px;\">\r\n"
  "</div>\r\n"
  "<div style=\"float : left; margin-left : 32px; font-size : 28px; height : 20px; margin-top : 75px;\">\r\n"
  "<span>%s</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
#endif
  "<div style=\"margin-top : 4px;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 500px; height : 0px; border : solid 1px #ccc;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; width : 128px; margin-left : 64px;\">%s</div>\r\n"
  "<div style=\"float : left; width : 340px; margin-top : 32px; margin-right : 32px;\"><span style=\"font-size : 24px; line-height : 26px;\">An application would like to connect to your account.</span></div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"font-size : 20px; width : 500px; margin-left : auto; margin-right : auto;\">\r\n"
  "<span>Sign in to authorize</span> <span style=\"font-weight : bold;\">%s</span> <span>access to your account.</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"text-align : center; margin-left : auto; margin-right : auto; width : 400px; background-color : rgba(220, 220, 220, 0.32);\"><span style=\"color : #dfcfdf; text-shadow : 2px 2px #090909;\">%s</span></div>\r\n"
  "\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "\r\n"
  "<div style=\"clear : both;\">\r\n"
  "\r\n"
  "<form action=\"/auth\">\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 400px;\">\r\n"
  "<div style=\"float : left; margin-top : 14px; margin-right : 8px;\"><span style=\"font-size : 16px;\">Username:&nbsp;</span></div>\r\n"
  "<input id=\"username\" name=\"username\" type=\"text\" style=\"width : 250px; height : 35px; margin-top : 7px; margin-right : 16px; font-size : 14px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"\" placeholder=\"Email Address\"></input> \r\n"
  "</div>\r\n"
  "\r\n"
  "\r\n"
  "<div style=\"clear : both;\">\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 400px;\">\r\n"
  "<div style=\"float : left; margin-top : 14px; margin-right : 12px;\"><span style=\"font-size : 16px;\">Password:&nbsp;</span></div>\r\n"
  "<input id=\"password\" name=\"password\" type=\"password\" style=\"width : 250px; height : 35px; margin-top : 7px; margin-right : 16px; font-size : 14px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"\" placeholder=\"Account Password\"></input> \r\n"
  "</div>\r\n"
  "\r\n"
  "<input type=\"hidden\" name=\"response_type\" value=\"password\">\r\n"
  "<input type=\"hidden\" name=\"client_id\" value=\"%s\">\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 32px;\"></div>\r\n"
  "\r\n"
  "\r\n"
  "<div style=\"float : right; margin-right : 32px;\">\r\n"
  "<input id=\"submit\" name=\"submit\" type=\"submit\" style=\"width : 150px; height : 35px; font-size : 18px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : -2px -2px -2px #D9D9D9; line-height : 25px;\" value=\"Login  \">\r\n"
  "</div>\r\n"
  "\r\n"
	"<div style=\"width : 150px; margin-left : 16px;\">\r\n"
	"<div style=\"position : absolute; transform : rotate(270deg); margin-left : auto; margin-right : auto; width : 60px; text-align : center; margin-top : 10px; margin-bottom : 2px; border-bottom : thick dotted #efefef;\"><span style=\"color : #efefef; font-weight : bold;\">2FA</span></div>\r\n"
	"<div style=\"background-color : #1f1fef; border-radius : 8px; margin-left : 12px; font-size : 10px; width : 140px;\">\r\n"
	"<div style=\"margin-left : 2px; padding-left : 16px; margin-top : 2px; height : 32px; padding-top : 8px; text-align : center; font-size : 14px; border : 2px solid #efefef; border-radius : 4px;\">\r\n"
	"<input type=\"radio\" name=\"enable_2fa\" value=\"on\" style=\"position : absolute; width : 62px;\"%s><span style=\"background-color : #efefef; color : #1f1fef; margin : 2px 2px 2px 4px; padding : 2px 4px 2px 4px; font-weight : bold;\">On&nbsp;&nbsp;&nbsp;</span></input>\r\n"
	"<input type=\"radio\" name=\"enable_2fa\" value=\"off\" style=\"position : absolute; width : 62px;\"%s><span style=\"margin : 2px 2px 2px 2px; padding : 2px 4px 2px 4px; font-weight : bold;\">Off&nbsp;&nbsp;&nbsp;</span></input>\r\n"
	"</div>\r\n"
	"<div style=\"clear : both;\"></div>\r\n"
	"</div>\r\n"
	"</div>\r\n"
  "</form>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 8px;\"></div>\r\n"
  "\r\n"
  "<br>\r\n"
  "\r\n"
  "</div>\r\n"
  "\r\n"
  "\r\n"
  "</body>\r\n"
  "</html>\r\n";

const char *_person_icon =
  "<img src=\"data:image/png;base64,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\" width=128 height=128 border=0>\r\n";

static const char *_neo_natura_logo = 
  "<img src=\"data:image/jpeg;base64,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\" width=48 height=48>\r\n";


static const char *_oauth_response_2fa_html =
  "<html>\r\n"
  "\r\n"
  "<body style=\"background-color : rgba(27,75,178,0.5); font-family : Helvetica,Arial,sans-serif,Georgia; padding : 0 0 0 0; color : #474a54; -webkit-font-smoothing : antialiased; font-size : 14px; line-height : 20px;\">\r\n"
  "\r\n"
  "<div style=\"margin-top : 8%%;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 600px; background-color : #000; color : #efefef; margin-top : 32px; box-shadow : 4px 4px 4px #1b1b1b; border-radius : 18px;\">\r\n"
  "\r\n"
  "<div style=\"float : right; margin-right : 48px; margin-top : -24px;\">\r\n"
  "%s\r\n"
  "</div>\r\n"
  "%s\r\n" /* header */
#if 0
  "<div style=\"clear : both;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; margin-left : 32px; padding : 16px 16px 16px 16px;\">\r\n"
  "<img src=\"%s\" style=\"max-width : 140px; max-height : 80px\">\r\n"
  "</div>\r\n"
  "<div style=\"float : left; margin-left : 32px; font-size : 28px; height : 20px; margin-top : 75px;\">\r\n"
  "<span>%s</span>\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
#endif
  "<div style=\"margin-top : 4px;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 500px; height : 0px; border : solid 1px #ccc;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
  "<div style=\"float : left; width : 128px; margin-left : 64px;\">%s</div>\r\n"
  "<div style=\"float : left; width : 340px; margin-top : 32px; margin-right : 32px;\"><span style=\"font-size : 24px; line-height : 26px;\">An application would like to connect to your account.</span></div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 16px;\"></div>\r\n"
  "\r\n"
	"<div style=\"font-size : 20px; width : 500px; margin-left : auto; margin-right : auto;\">\r\n"
	"<span>Enter the Two-factor Authentification code to access</span>\r\n"
	"<span>your account.</span>\r\n"
	"</div>\r\n"
	"\r\n"
	"<div style=\"margin-top : 32px;\"></div>\r\n"
	"\r\n"
  "<form action=\"/auth\">\r\n"
	"\r\n"
	"<div style=\"clear : both;\">\r\n"
	"\r\n"
	"<div style=\"margin-left : auto; margin-right : auto; width : 500px;\">\r\n"
	"<div style=\"float : left; margin-top : 14px; margin-right : 8px;\"><span style=\"font-size : 16px;\">Authentification code:&nbsp;</span></div>\r\n"
	"<div style=\"clear : both;\">\r\n"
	"<input id=\"2fa\" name=\"2fa\" type=\"text\" style=\"width : 250px; height : 35px; margin-top : 7px; margin-right : 16px; font-size : 14px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"\" placeholder=\"000000\"></input>       \r\n"
	"</div>\r\n"
	"\r\n"
  "<input type=\"hidden\" name=\"response_type\" value=\"2fa\">\r\n"
  "<input type=\"hidden\" name=\"client_id\" value=\"%s\">\r\n"
  "<input type=\"hidden\" name=\"code\" value=\"%s\">\r\n"
	"\r\n"
	"<div style=\"clear : both;\"></div>\r\n"
	"<div style=\"margin-top : 16px;\"></div>\r\n"
	"\r\n"
	"<div style=\"float : right;\">\r\n"
	"<input id=\"submit\" name=\"submit\" type=\"submit\" style=\"width : 140px; height : 35px; font-size : 18px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"Verify  \">\r\n"
	"</div>\r\n"
  "\r\n"
  "</form>"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 8px;\"></div>\r\n"
  "\r\n"
  "<br>\r\n"
  "\r\n"
  "</div>\r\n"
  "\r\n"
  "\r\n"
  "</body>\r\n"
  "</html>\r\n";

static const char *_oauth_register_2fa_html =
  "<html>\r\n"
  "\r\n"
  "<body style=\"background-color : rgba(27,75,178,0.5); font-family : Helvetica,Arial,sans-serif,Georgia; padding : 0 0 0 0; color : #474a54; -webkit-font-smoothing : antialiased; font-size : 14px; line-height : 20px;\">\r\n"
  "\r\n"
  "<div style=\"margin-top : 8%%;\"></div>\r\n"
  "\r\n"
  "<div style=\"margin-left : auto; margin-right : auto; width : 600px; background-color : #000; color : #efefef; margin-top : 32px; box-shadow : 4px 4px 4px #1b1b1b; border-radius : 18px;\">\r\n"
  "\r\n"
  "<div style=\"float : right; margin-right : 48px; margin-top : -24px;\">\r\n"
  "%s\r\n"
  "</div>\r\n"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "\r\n"
	"<div style=\"clear : both;\"></div>\r\n"
	"\r\n"
	"<div style=\"margin-top : 16px;\"></div>\r\n"
	"\r\n"
  "<div style=\"float : left; width : 128px; margin-left : 64px;\">%s</div>\r\n"
	"<div style=\"float : left; width : 340px; margin-top : 32px; margin-right : 32px;\"><span style=\"font-size : 24px; line-height : 26px;\">Link your account to a two-factor authentication app.</span></div>\r\n"
	"\r\n"
	"<div style=\"clear : both;\"></div>\r\n"
	"<div style=\"margin-top : 16px;\"></div>\r\n"
	"\r\n"
  "<form action=\"/auth\">\r\n"
	"\r\n"
	"<div style=\"font-size : 20px; width : 500px; margin-left : auto; margin-right : auto;\">\r\n"
	"<span>Scan the QR Code with Google Authenticator or enter the 2FA private key.</span>\r\n"
	"</div>\r\n"
	"\r\n"
	"<div style=\"float : right; margin-right : 32px; padding : 2px 2px 2px 2px; background-color : #ffffff;\">\r\n"
	"<img src=\"%s\" style=\"max-width : 140; max-height : 80px\">\r\n"
	"</div>\r\n"
	"\r\n"
	"<div style=\"margin-top : 64px; margin-left : 64px;\">\r\n"
	"<span>Private Key ( </span>\r\n"
	"<span><i>%s</i></span>\r\n"
	"<span> ): </span>\r\n"
	"<br>\r\n"
	"<span style=\"font-weight : bold; font-size : 16px; font-variant : small-caps; font-family : Georgia;\">%s</span>\r\n"
	"</div>\r\n"
	"\r\n"
	"<div style=\"margin-top : 70px;\">\r\n"
	"\r\n"
	"<div style=\"margin-left : auto; margin-right : auto; width : 500px;\">\r\n"
	"<div style=\"float : left; margin-top : 14px; margin-right : 8px;\"><span style=\"font-size : 14px;\">Enter the six-digit code from the application.</span></div>\r\n"
	"<div style=\"clear : both;\">\r\n"
	"<input id=\"2fa\" name=\"2fa\" type=\"text\" style=\"width : 250px; height : 35px; margin-top : 7px; margin-right : 16px; font-size : 14px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"\" placeholder=\"000000\"></input>\r\n"
	"</div>\r\n"
	"\r\n"
	"\r\n"
	"\r\n"
	"<div style=\"clear : both;\"></div>\r\n"
	"<div style=\"margin-top : 32px;\"></div>\r\n"
	"\r\n"
	"\r\n"
	"<div style=\"float : right; margin-right : 16px;\">\r\n"
	"<input id=\"verify\" name=\"verify\" type=\"submit\" style=\"width : 140px; height : 35px; font-size : 18px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"Verify  \">\r\n"
	"</div>\r\n"
	"\r\n"
  "<input type=\"hidden\" name=\"response_type\" value=\"2fa\">\r\n"
  "<input type=\"hidden\" name=\"enable_2fa\" value=\"on\">\r\n"
  "<input type=\"hidden\" name=\"client_id\" value=\"%s\">\r\n"
  "<input type=\"hidden\" name=\"code\" value=\"%s\">\r\n"
	"\r\n"
	"<div style=\"float : right; margin-right : 16px;\">\r\n"
	"<input id=\"skip\" name=\"skip\" type=\"submit\" style=\"width : 140px; height : 35px; font-size : 18px; color : #444; outline : medium none; padding-left : 20px; border-radius : 6px; box-shadow : 0px 2px 0px #D9D9D9; line-height : 25px;\" value=\"Skip  \">\r\n"
	"</div>\r\n"
  "</form>"
  "\r\n"
  "<div style=\"clear : both;\"></div>\r\n"
  "<div style=\"margin-top : 8px;\"></div>\r\n"
  "\r\n"
  "<br>\r\n"
  "\r\n"
  "</div>\r\n"
  "\r\n"
  "\r\n"
  "</body>\r\n"
  "</html>\r\n";


#endif /* ifdef __OAUTH_TEMPLATE_C__ */


#endif /* ndef __OAUTH_TEMPLATE_H__ */



