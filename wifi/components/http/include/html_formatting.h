#pragma once

#ifndef __HTML_FORMATTING_H__
#define __HTML_FORMATTING_H__



#define CSS_CLASS_GROUP_RND "group rnd"



#define BLOCK_S           "<"
#define BLOCK_SS           "</"
#define BLOCK_E           ">"

#define HQ          "'"

#define BOLD_S      BLOCK_S "b" BLOCK_E
#define BOLD_E      BLOCK_SS "b" BLOCK_E

#define _DIV_         "div"
#define DIV_S_        BLOCK_S _DIV_   // "<div"
#define DIV_S         BLOCK_S _DIV_ BLOCK_E   // "<div>"
#define DIV_S_E       BLOCK_E         // ">"
#define DIV_S_Q_E     HQ BLOCK_E      // "'>"
#define DIV_E         BLOCK_SS _DIV_ BLOCK_E  // "</div>"

#define _P_           "p"
#define P_S          BLOCK_S _P_ BLOCK_E  // "<p>"
#define P_S_         BLOCK_S _P_   // "<p"
#define P_S_Q_E      HQ   BLOCK_E   // " '>"
#define P_E          BLOCK_SS _P_ BLOCK_E  //"</p>"

#define _H2_           "h2"
#define H2_S          BLOCK_S _H2_ BLOCK_E  // "<h2>"
#define H2_E          BLOCK_SS _H2_ BLOCK_E  //"</h2>"

#define _H4_           "h4"
#define H4_S          BLOCK_S _H4_ BLOCK_E  // "<h4>"
#define H4_S_          BLOCK_S _H4_   // "<h4"
#define H4_S_Q_E      HQ BLOCK_E      // "'>"
#define H4_E          BLOCK_SS _H4_ BLOCK_E  //"</h4>"

#define _SPAN_          "span"
#define SPAN_S_          BLOCK_S _SPAN_               // "<span"
#define SPAN_S           BLOCK_S _SPAN_ BLOCK_E              // "<span>"
#define SPAN_E           BLOCK_SS _SPAN_ BLOCK_E      //"</span>"

#define CSS_CLASS           " class='"
#define CSS_ID              " id='"

#define _BUTTON_            "button"
#define BUTTON_S_           BLOCK_S _BUTTON_            //"<button"
#define BUTTON_S            BLOCK_S _BUTTON_ BLOCK_E          // "<button>"
#define BUTTON_S_E         BLOCK_E                  // ">"
#define BUTTON_S_Q_E        HQ BLOCK_E                  // "'>"
#define BUTTON_E            BLOCK_SS _BUTTON_ BLOCK_E   //"</button>"

#define _UL_               "ul"
#define UL_S_             BLOCK_S _UL_ //"<ul"
#define UL_S              BLOCK_S _UL_ BLOCK_E // <ul>
#define UL_S_Q_E          HQ BLOCK_E   // "'>"
#define UL_E              BLOCK_SS _UL_ BLOCK_E //"</ul>"

#define _LI_               "li"
#define LI_S_             BLOCK_S _LI_ //"<li"
#define LI_S              BLOCK_S _LI_ BLOCK_E // <li>
#define LI_S_Q_E          HQ BLOCK_E   // "'>"
#define LI_E              BLOCK_SS _LI_ BLOCK_E //"</li>"

#define _ANC_ "a href='"  // <a href='%s'>%s</a>
#define ANC_S_   BLOCK_S _ANC_  // "<a href='"
#define ANC_S_Q_E   HQ BLOCK_E  // "'>'"
#define ANC_E    BLOCK_SS "a" BLOCK_E // </a>

#define _LABEL_ "label"
#define LABEL_S BLOCK_S _LABEL_ BLOCK_E // <label>
#define LABEL_S_ BLOCK_S _LABEL_ // <label 
#define LABEL_S_Q_E HQ BLOCK_E  // "'>"
#define LABEL_E BLOCK_SS _LABEL_ BLOCK_E // "</label>"


#define _INPUT_ "input"
#define INPUT_S BLOCK_S _INPUT_ BLOCK_E // <input>
#define INPUT_S_ BLOCK_S _INPUT_ // <input 
#define INPUT_S_Q_E HQ BLOCK_E  // "'>"
#define INPUT_SS_E " /" BLOCK_E  // " />"
#define INPUT_E BLOCK_SS _INPUT_ BLOCK_E // "</input>"
#define INPUT_TYPE " type='"
#define INPUT_NAME " name='"
#define INPUT_VALUE " value='"
#define INPUT_SIZE " size='"

#define _FORM_ "form"
#define FORM_S BLOCK_S _FORM_ BLOCK_E // <form>
#define FORM_S_ BLOCK_S _FORM_ // <form 
#define FORM_S_Q_E HQ BLOCK_E  // "'>"
#define FORM_S_E  BLOCK_E  // "'>"
#define FORM_E BLOCK_SS _FORM_ BLOCK_E // "</form>"

#define FORM_ENCTYPE " enctype='multipart/form-data'" 
#define FORM_METHOD " method=" 
#define FORM_GET  FORM_METHOD "'get'"
#define FORM_POST FORM_METHOD "'post'"
#define FORM_ACTION " action=" 

#endif