<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="layout/database.css" />
    <title>LOFAR Cosmic Ray Events</title>
    <script src="layout/sorttable.js"></script>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Events</h1>
  <nav>
      <ul class="cf">
          <li><a class="dropdown" href="#">Section</a>
              <ul>
                  <li><a href="#good">Good events</a></li>
                  <li><a href="#all">All events</a></li>
              </ul>
          </li>
          <li><a href="/events">All events</a></li>
      </ul>
  </nav>
  <table class="sortable">
    <caption id="good">Good events</caption>
    <thead>
      <tr>
        <th>Id</th>
        <th>Timestamp</th>
        <th>Event status</th>
        <th>Event alt_status</th>
        <th>Energy (eV)</th>
        <th>Core x (m)</th>
        <th>Core y (m)</th>
        <th>Azimuth (deg)</th>
        <th>Elevation (deg)</th>
        <th>Moliere radius (m)</th>
      </tr>
    </thead>
    <xsl:for-each select="/elements/event">
      <xsl:if test="status='CR_FOUND' or alt_status='CR_FOUND'">
      <xsl:choose>
         <xsl:when test="lora/@good_reconstruction">
           <tr class="good">
             <td>
               <a><xsl:attribute name="href">
               events/<xsl:value-of select="id"/></xsl:attribute> 
               <xsl:value-of select="id"/> 
               </a>
             </td>
             <td><xsl:value-of select="timestamp"/></td>
             <td><xsl:value-of select="status"/></td>
             <td><xsl:value-of select="alt_status"/></td>
             <td><xsl:value-of select="lora/energy"/></td>
             <td><xsl:value-of select="lora/core_x"/></td>
             <td><xsl:value-of select="lora/core_y"/></td>
             <td><xsl:value-of select="lora/azimuth"/></td>
             <td><xsl:value-of select="lora/elevation"/></td>
             <td><xsl:value-of select="lora/moliere"/></td>
           </tr>
         </xsl:when>
         <xsl:when test="status='ERROR' or alt_status='ERROR'">
           <tr class="error">
             <td>
               <a><xsl:attribute name="href">
               events/<xsl:value-of select="id"/></xsl:attribute> 
               <xsl:value-of select="id"/> 
               </a>
             </td>
             <td><xsl:value-of select="timestamp"/></td>
             <td><xsl:value-of select="status"/></td>
             <td><xsl:value-of select="alt_status"/></td>
             <td><xsl:value-of select="lora/energy"/></td>
             <td><xsl:value-of select="lora/core_x"/></td>
             <td><xsl:value-of select="lora/core_y"/></td>
             <td><xsl:value-of select="lora/azimuth"/></td>
             <td><xsl:value-of select="lora/elevation"/></td>
             <td><xsl:value-of select="lora/moliere"/></td>
           </tr>
         </xsl:when>
         <xsl:otherwise>
           <tr>
             <td>
               <a><xsl:attribute name="href">
               events/<xsl:value-of select="id"/></xsl:attribute> 
               <xsl:value-of select="id"/> 
               </a>
             </td>
             <td><xsl:value-of select="timestamp"/></td>
             <td><xsl:value-of select="status"/></td>
             <td><xsl:value-of select="alt_status"/></td>
             <td><xsl:value-of select="lora/energy"/></td>
             <td><xsl:value-of select="lora/core_x"/></td>
             <td><xsl:value-of select="lora/core_y"/></td>
             <td><xsl:value-of select="lora/azimuth"/></td>
             <td><xsl:value-of select="lora/elevation"/></td>
             <td><xsl:value-of select="lora/moliere"/></td>
           </tr>
         </xsl:otherwise>
      </xsl:choose>
      </xsl:if>
    </xsl:for-each>
  </table>
  <table class="sortable">
    <caption id="all" class="parameters">All events</caption>
    <tr>
      <th>Id</th>
      <th>Timestamp</th>
      <th>Event status</th>
      <th>Event alt_status</th>
      <th>Energy (eV)</th>
      <th>Core x (m)</th>
      <th>Core y (m)</th>
      <th>Azimuth (deg)</th>
      <th>Elevation (deg)</th>
      <th>Moliere radius (m)</th>
    </tr>
    <xsl:for-each select="/elements/event">
      <tr>
        <td>
          <a><xsl:attribute name="href">
          events/<xsl:value-of select="id"/></xsl:attribute> 
          <xsl:value-of select="id"/> 
          </a>
        </td>
        <td><xsl:value-of select="timestamp"/></td>
        <td><xsl:value-of select="status"/></td>
        <td><xsl:value-of select="alt_status"/></td>
        <td><xsl:value-of select="lora/energy"/></td>
        <td><xsl:value-of select="lora/core_x"/></td>
        <td><xsl:value-of select="lora/core_y"/></td>
        <td><xsl:value-of select="lora/azimuth"/></td>
        <td><xsl:value-of select="lora/elevation"/></td>
        <td><xsl:value-of select="lora/moliere"/></td>
      </tr>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>

