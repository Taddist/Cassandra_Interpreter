#ifndef CASSANDRA_H
#define CASSANDRA_H
typedef enum {
				COMMENTAIRE,
				ALTER,
				DELETE,
				DROP,
				ERREUR,
				IDENTIFICATEUR,
				POINT,
				APOSTROPHE,
				SEMICOLON,
				PAR_OUV,
				PAR_FER,
				ACCO_OUV,
				VIRGULE,
				FUNCTION,
				AGGREGATE,
				TYPE,
				TABLE,
				USER,
				KEYSPACE,
				ROLE,
				TRIGGER,
				INDEX,
				IF_TOKEN,
				EXISTS_TOKEN,
				MATERIALIZED,
				VIEW,
				INSERT,
				INTO,
				INTEGER_TOKEN,
				FLOAT_TOKEN,
				NAN,
				INFINITY,
				NULL_TOKEN,
				TRUE_TOKEN,
				FALSE_TOKEN,
				UUID_TOKEN,
				BLOB_TOKEN,
				STRING_TOKEN,
				DOUBLE_QUOTE,
				VALUES_TOKEN,
				JSON,
				NOT_TOKEN,
				USING,
				HEX_TOKEN,
				TIMESTAMP,
				TTL,
				AND,
				CRO_FER,
				CRO_OUV,
				ACCO_FER,
				POINT_INTERO,
				DEUX_POINT,
				EGAL,
				WITH,
				CREATE,
				USE,
				ON,
				ASCII_T,
				BIGINT_T,
				BLOB_T,
				BOOLEAN_T,
				COUNTER_T,
				DATE_T,
				DECIMAL_T,
				DOUBLE_T,
				FLOAT_T,
				INT_T,
				TEXT_T,
				INET_T,
				SMALLINT_T,
				TIME_T,
				TIMEUUID_T,
				TINYINT_T,
				UUID_T,
				VARCHAR_T,
				VARINT_T,
				MAP_TOKEN,
				LIST_TOKEN,
				SET_TOKEN,
				TUPLE_TOKEN,
				INF,
				SUP,
				COLUMNFAMILY,
				STATIC,
				PRIMARY,
				KEY,
				ORDER,
				CLUSTERING,
				COMPACT,
				STORAGE,
				CUSTOM,
				KEYS,
				OPTIONS,
				SELECT,
				ETOILE,
				COUNT_T,
				UN,
				AS_T,
				FROM_T,
				DISTINCT,
				LIMIT,
				ALLOW,
				WHERE,
				INFE,
				SUPE,
				CONTAINS,
				CONTAINSKEY,
				FILTERING,
				ASC,
				DESC,
				BY_T,
				WRITETIME_T,
				IN,
				TOKEN,
				UPDATE,
				DELETE_T,
				BEGIN_T,
				BATCH,
				APPLY_T,
				UNLOGGED,

}typetoken;

typedef enum {false=0, true=1} boolean;



typetoken _lire_token();
boolean _drop_stmt();
boolean _drop_type();
boolean _view();
boolean _if_exists();
boolean _drop_aux();
boolean _table_name();
boolean _term();
boolean _collection_liter();
boolean _option();
boolean _list_term_or_liter();
boolean _value_list();
boolean _name_list();
boolean _option_list_aux();
boolean _insert_stmt();
boolean _list_json();
boolean _if_not_ex();
boolean _list_idf();
boolean _list_idf_aux();
boolean _idf();
boolean _constant();
boolean _float();
boolean _boolean();
boolean _string();
boolean _option_x();
boolean _option_list();
boolean _table_name_aux();
boolean _term_list_x();
boolean _term_list();
boolean _term_list_aux();
boolean _variable();
boolean  _collection_liter();
boolean _map_liter();
boolean _term_term_list_x();
boolean _term_term_list();
boolean _term_term_list_aux();
boolean _set_liter();
boolean _list_liter();
boolean _drop();
boolean _property_aux();
boolean _property();
boolean _create_keyspace();
boolean _if_not_exist();
boolean _properties();
boolean _properties_aux();
boolean _use_stmt();
boolean _drop_idfp();
boolean _idf_p_idf();
boolean _idf_p_idf_aux();
boolean _type_t();
boolean _drop_trigger();
boolean _function_aux();
boolean _arg_sign();
boolean _arg_type();
boolean _arg_sign_aux();
boolean _drop_function_aggregate();
boolean _drop_list();
boolean _native_type();
boolean _collection_type();
boolean _user_defined_type();
boolean _tuple_type();
boolean _custom_type();
boolean _tuple_type_aux();
boolean _tuple_type_x();
boolean _drop_stmt();
boolean _list_term_or_liter_aux();
boolean _list_literal();
boolean _map_literal_aux();
boolean _set_literal_aux();
boolean _map_set_literal();
boolean _list_literal_aux();
boolean _create_stmt();
boolean _map_literal();
boolean _map_literal_aux();
boolean _map_literal();
boolean _create_stmt();
boolean _create_keyspace();
boolean _map_lit();
boolean _map_lit_aux();
boolean _map_lit_x();
boolean _create_table();
boolean _tab_col();
boolean _list_column_df();
boolean _list_column_df_aux();
boolean _column_df();
boolean _column_df_i() ;
boolean _column_df_p();
boolean _static();
boolean _primary_key();
boolean _type();
boolean _list_part_key();
boolean _part_key();
boolean _part_key_aux();
boolean _part_key_x();
boolean _list_id();
boolean _list_option();
boolean _compact_storage();
boolean _clustering_order();
boolean _create_index();
boolean _custom();
boolean _index_n();
boolean _index_idf();
boolean _using_stmt();
boolean _with_stmt();
boolean _select_stmt();
boolean _json();
boolean _select_clause();
boolean _et_un();
boolean _as_idf_ch();
boolean _distinct_ch();
boolean _select_list();
boolean _selector_list();
boolean _selector_list_aux();
boolean _list_selector_aux();
boolean _list_selector();
boolean _selector();
boolean _where_ch();
boolean _where_cl();
boolean _where_cl_aux();
boolean _relation();
boolean _op();
boolean _allow_ch();
boolean _limit_ch();
boolean _ordering();
boolean _ordering_aux();
boolean _order_by_aux();
boolean _order_by();
boolean _order_by_ch();
boolean _idf_rl();
boolean _idf_rl_aux();
boolean _term_list_ch();
boolean _update_stmt();
boolean _if_ch();
boolean _using_ch();
boolean _list_assignment();
boolean _list_options();
boolean _list_option_aux();
boolean _assignment();
boolean _list_assignment_aux();
boolean _list_cond();
boolean _list_cond_aux();
boolean _condition();
boolean _delete_stmt();
boolean _list_selection_ch();
boolean _list_selection();
boolean _list_selection_aux();
boolean _selection();
boolean _term_ch();
boolean _using_times_ch();
boolean _if_stmt_ch();
boolean _if_stmt();
boolean _fct_agg();
boolean _modif_stmt();
boolean _modif_stmt_list_aux();
boolean _modif_stmt_list();
boolean _opt_list_aux();
boolean _option_timestamp();
boolean _opt_list();
boolean _using_opt_list();
boolean _unlo_coun();
boolean _batch_stmt();

#endif

