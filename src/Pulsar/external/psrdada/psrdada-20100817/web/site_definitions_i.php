<?PHP

/* Directory configuration */

define(DADA_ROOT,  "/home/dada/linux_64");
define(WEB_BASE,   DADA_ROOT."/web/".INSTRUMENT);
define(URL_BASE,   "/web/".INSTRUMENT);

define(STATUS_FILE_DIR, WEB_BASE."/status");
define(PLOT_DIR,        WEB_BASE."/plots");
define(RESULTS_DIR,     WEB_BASE."/results");

/* Config files */
define(SYS_CONFIG,         DADA_ROOT."/share/apsr.cfg");
define(DADA_CONFIG,        DADA_ROOT."/share/apsr_tcs.cfg");
define(DADA_SPECIFICATION, DADA_ROOT."/share/apsr_tcs.spec");

/* Time/date configuration */
define(DADA_TIME_FORMAT,  "Y-m-d-H:i:s");
define(UTC_TIME_OFFSET,   10);
define(LOCAL_TIME_OFFSET, (UTC_TIME_OFFSET*3600));

define(STATUS_OK,    "0");
define(STATUS_WARN,  "1");
define(STATUS_ERROR, "2");

define(LOG_FILE_SCROLLBACK_HOURS,6);
define(LOG_FILE_SCROLLBACK_SECS,LOG_FILE_SCROLLBACK_HOURS*60*60);

define(FAVICO_HTML,         "<link rel='shortcut icon' href='/images/favicon.ico'/>");

define(BANNER_IMAGE,        "/images/apsr_logo_480x60.png");
define(BANNER_IMAGE_REPEAT, "/images/apsr_logo_1x60.png");

?>
