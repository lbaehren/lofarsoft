<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="/layout/database.css" />
    <title>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/></title>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/></h1>
  <nav>
      <ul class="cf">
          <li><a class="dropdown" href="#">Section</a>
              <ul>
                  <li><a href="#header">Header</a></li>
                  <li><a href="#logs">Logs</a></li>
                  <li><a href="#stations">Stations</a></li>
                  <li><a href="#datafiles">Datafiles</a></li>
                  <li><a href="#figures">Figures</a></li>
                  <li><a href="#parameters">Parameters</a></li>
              </ul>
          </li>
          <li><a href="/events">All events</a></li>
          <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute>Current Event</a></li>
          <!--
          <li><a class="dropdown" href="#">Datafiles</a>
              <ul>
                <xsl:for-each select="/elements/header/datafiles/datafile">
                <xsl:sort select="name"/>
                  <li><a href="#"><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
          -->
          <li><a class="dropdown" href="#">Stations</a>
              <ul>
                <xsl:for-each select="/elements/header/stations/station">
                <xsl:sort select="name"/>
                  <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
      </ul>
  </nav>
  <table>
  <caption id="header">Header</caption>
    <tr>
      <th>Id</th>
      <th>Timestamp</th>
      <th>Event status</th>
    </tr>
    <tr>
      <td>
        <td><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute><xsl:value-of select="/elements/header/id"/></a></td>
      </td>
      <td>
        <td><xsl:value-of select="/elements/header/timestamp"/></td>
      </td>
      <td>
        <td><xsl:value-of select="/elements/header/status"/></td>
      </td>
    </tr>
  </table>
  <table>
  <caption id="logs">Logs</caption>
  <tr><th>Name</th><th>Description</th></tr>
      <xsl:if test="/elements/header/status='CR_FOUND' or /elements/header/status='CR_NOT_FOUND'">
        <tr>
          <td><a><xsl:attribute name="href">/log/cr_event-<xsl:value-of select="/elements/header/id"/>.log</xsl:attribute>cr_event</a></td>
          <td>First stage pipeline</td>
        </tr>
      </xsl:if>
      <xsl:if test="/elements/header/status='CR_ANALYZED' or /elements/header/status='CR_NOT_ANALYZED'">
        <tr>
          <td><a><xsl:attribute name="href">/log/cr_event-<xsl:value-of select="/elements/header/id"/>.log</xsl:attribute>cr_event</a></td>
          <td>First stage pipeline</td>
        </tr>
        <tr>
          <td><a><xsl:attribute name="href">/log/cr_event-<xsl:value-of select="/elements/header/id"/>.log</xsl:attribute>cr_physics</a></td>
          <td>Second stage pipeline</td>
        </tr>
      </xsl:if>
  </table>
  <table>
  <caption id="stations">Stations</caption>
  <tr>
    <th>Name</th>
    <th>Station status</th>
    <th>Polarization</th>
    <th>Polarization status</th>
  </tr>
    <xsl:for-each select="/elements/header/stations/station">
    <xsl:sort select="name"/>
      <tr>
        <xsl:variable name="station"><xsl:value-of select="name"/></xsl:variable>
        <td><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:copy-of select="$station" /></xsl:attribute><xsl:copy-of select="$station" /></a></td>
        <td><xsl:value-of select="status"/></td>
        <td>
          <xsl:for-each select="polarizations/polarization">
            <tr>
              <td><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:copy-of select="$station" />/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></td>
            </tr>
          </xsl:for-each>
        </td>
        <td>
          <xsl:for-each select="polarizations/polarization">
            <tr>
              <td><xsl:value-of select="status"/></td>
            </tr>
          </xsl:for-each>
        </td>
      </tr>
    </xsl:for-each>
  </table>
  <table class="parameters">
    <caption id="datafiles">Datafiles</caption>
    <tr>
      <th>Filename</th>
    </tr>
    <xsl:for-each select="/elements/header/datafiles/datafile">
    <xsl:sort select="name"/>
      <tr>
        <td><xsl:value-of select="name"/></td>
      </tr>
    </xsl:for-each>
  </table>
  <table>
    <caption id="figures">Figures</caption>
    <xsl:for-each select="/elements/figures/figure">
      <tr>
        <td>
          <a><xsl:attribute name="href"><xsl:value-of select="path"/></xsl:attribute>
            <img><xsl:attribute name="src"><xsl:value-of select="path"/></xsl:attribute><xsl:attribute name="alt"><xsl:value-of select="path"/></xsl:attribute></img>
          </a>
        </td>
      </tr>
    </xsl:for-each>
  </table>
  <table class="parameters">
    <caption id="parameters">Parameters</caption>
    <tr>
      <th>key</th>
      <th>value</th>
    </tr>
    <xsl:for-each select="/elements/parameters/parameter">
      <xsl:sort select="key"/>
      <tr>
        <td><xsl:value-of select="key"/></td>
        <td><xsl:value-of select="value"/></td>
      </tr>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>

