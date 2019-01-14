#ifndef _es_core_db_utilities_h_
#define _es_core_db_utilities_h_

namespace EsUtilities
{

// DB field generic customization
template <typename FieldT>
void dbFieldConfig(TDataSet* ds, const UnicodeString& name, const UnicodeString& label,
  const UnicodeString& fmtDisplay)
{
  FieldT* fld = dynamic_cast<FieldT*>(ds->FieldByName(name));
  ES_ASSERT(fld);

  fld->DisplayLabel = label;
  fld->DisplayFormat = fmtDisplay;
}

template <typename FieldT>
void dbFieldConfig(TDataSet* ds, const UnicodeString& name, const UnicodeString& label,
  const UnicodeString& fmtDisplay, const UnicodeString& fmtEdit)
{
  FieldT* fld = dynamic_cast<FieldT*>(ds->FieldByName(name));
  ES_ASSERT(fld);

  fld->DisplayLabel = label;
  fld->DisplayFormat = fmtDisplay;
  fld->EditFormat = fmtEdit;
}

}

#endif // _es_core_db_utilities_h_

