<xsl:stylesheet version="1.0"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:tcx="http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2"
        xmlns:tpx="http://www.garmin.com/xmlschemas/ActivityExtension/v2">

  <xsl:output method="text" encoding="iso-8859-1"/>
  <xsl:strip-space elements="*" />

  <xsl:template match="/">
    <xsl:text>event_time, ref_speed_mph, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, flywheel, temperature, ref_torque, ref_wheel_rev</xsl:text>
    <xsl:text>&#10;</xsl:text>
    <xsl:apply-templates select="/tcx:TrainingCenterDatabase/tcx:Activities/tcx:Activity/tcx:Lap/tcx:Track" />
  </xsl:template>

  <xsl:template match="tcx:Trackpoint">
    <xsl:value-of select="tcx:Time" />,,,<xsl:value-of select="tcx:Extensions/tpx:TPX/tpx:Speed" />,,<xsl:value-of select="tcx:Extensions/tpx:TPX/tpx:Watts" />,,2000,,,,<xsl:text>&#10;</xsl:text>
  </xsl:template>
  
</xsl:stylesheet>
