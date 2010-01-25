WRAP_CLASS("itk::RealAndImaginaryToComplexImageFilter" POINTER_WITH_SUPERCLASS)
  FOREACH(d ${WRAP_ITK_DIMS})
    FOREACH(t ${WRAP_ITK_REAL})
#      FOREACH(c ${WRAP_ITK_COMPLEX_REAL})
#        WRAP_TEMPLATE("${ITKM_${t}}${ITKM_${t}}${ITKM_${c}}${d}" "${ITKT_${t}},${ITKT_${t}},${ITKT_${c}},${d}")
#      ENDFOREACH(c)

      IF(WRAP_complex_float)
         WRAP_TEMPLATE("${ITKM_${t}}${ITKM_${t}}${ITKM_F}${d}" "${ITKT_${t}},${ITKT_${t}},${ITKT_F},${d}")
      ENDIF(WRAP_complex_float)
      
      IF(WRAP_complex_double)
         WRAP_TEMPLATE("${ITKM_${t}}${ITKM_${t}}${ITKM_D}${d}" "${ITKT_${t}},${ITKT_${t}},${ITKT_L},${d}")
      ENDIF(WRAP_complex_double)
    ENDFOREACH(t)
  ENDFOREACH(d)
END_WRAP_CLASS()
