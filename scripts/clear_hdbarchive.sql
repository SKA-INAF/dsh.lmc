/* =========================================== */
-- ===        SELECT DB TABLE
/* =========================================== */
-- Use DB
-- SELECT CONCAT('INFO: Use Tango DB ',@db_name);
-- USE @db_name;

DELIMITER #


/* =========================================== */
-- ===   CREATE DELETE ATTR DATA PROCEDURE
/* =========================================== */
DROP procedure if exists delete_old_attr;
CREATE PROCEDURE delete_old_attr (IN attr VARCHAR(255), IN delete_date DATETIME)
proc_delete_attr: BEGIN
	DECLARE table_name VARCHAR(255);
	DECLARE att_id INT;
	DECLARE countRow INT;
	DECLARE totCountRow INT;
	DECLARE foundData INT;
	DECLARE att_conf_count INT;

	-- Check if attribute exists;
	SET att_conf_count = (SELECT count(*) FROM att_conf WHERE att_name = attr);
	SELECT att_conf_count;
	IF att_conf_count != 1 THEN
		SELECT CONCAT('WARN: Attribute not found: ',attr);
		LEAVE proc_delete_attr;
	END IF;

	SELECT att_conf_id, CONCAT('att_',data_type) FROM att_conf c JOIN att_conf_data_type d ON c.att_conf_data_type_id = d.att_conf_data_type_id WHERE att_name = attr INTO att_id, table_name;


	-- Delete data
	SELECT CONCAT(' ',table_name,'att_conf_id=',att_id,'Starting to delete from time:',delete_date);

	SET countRow= 1;

	WHILE countRow >0 DO
		REPEAT

/*
			IF table_name = 'att_scalar_devboolean_ro' THEN
				SET countRow= (SELECT COUNT(data_time) FROM att_scalar_devboolean_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE,delete_date,data_time)<0 LIMIT 10000);
			ELSEIF table_name = 'att_scalar_devfloat_ro' THEN
				SET countRow= (SELECT COUNT(data_time) FROM att_scalar_devfloat_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE,delete_date,data_time)<0 LIMIT 10000);
			ELSEIF table_name = 'att_scalar_devfloat_rw' THEN
				SET countRow= (SELECT COUNT(data_time) FROM att_scalar_devfloat_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE,delete_date,data_time)<0 LIMIT 10000);
			END IF;

			SELECT countRow;
*/


		IF table_name = 'att_scalar_devboolean_ro' THEN
				DELETE FROM att_scalar_devboolean_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devboolean_rw' THEN
				DELETE FROM att_scalar_devboolean_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devboolean_ro' THEN
				DELETE FROM att_array_devboolean_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devboolean_rw' THEN
				DELETE FROM att_array_devboolean_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devuchar_ro' THEN
				DELETE FROM att_scalar_devuchar_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devuchar_rw' THEN
				DELETE FROM att_scalar_devuchar_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devuchar_ro' THEN
				DELETE FROM att_array_devuchar_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devuchar_rw' THEN
				DELETE FROM att_array_devuchar_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devshort_ro' THEN
				DELETE FROM att_scalar_devshort_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devshort_rw' THEN
				DELETE FROM att_scalar_devshort_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devshort_ro' THEN
				DELETE FROM att_array_devshort_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devshort_rw' THEN
				DELETE FROM att_array_devshort_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devushort_ro' THEN
				DELETE FROM att_scalar_devushort_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devushort_rw' THEN
				DELETE FROM att_scalar_devushort_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devushort_ro' THEN
				DELETE FROM att_array_devushort_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devushort_rw' THEN
				DELETE FROM att_array_devushort_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong_ro' THEN
				DELETE FROM att_scalar_devlong_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong_rw' THEN
				DELETE FROM att_scalar_devlong_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong_ro' THEN
				DELETE FROM att_array_devlong_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong_rw' THEN
				DELETE FROM att_array_devlong_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong_ro' THEN
				DELETE FROM att_scalar_devulong_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong_rw' THEN
				DELETE FROM att_scalar_devulong_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong_ro' THEN
				DELETE FROM att_array_devulong_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong_rw' THEN
				DELETE FROM att_array_devulong_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong64_ro' THEN
				DELETE FROM att_scalar_devlong64_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong64_rw' THEN
				DELETE FROM att_scalar_devlong64_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong64_ro' THEN
				DELETE FROM att_array_devlong64_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong64_rw' THEN
				DELETE FROM att_array_devlong64_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong64_ro' THEN
				DELETE FROM att_scalar_devulong64_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong64_rw' THEN
				DELETE FROM att_scalar_devulong64_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong64_ro' THEN
				DELETE FROM att_array_devulong64_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong64_rw' THEN
				DELETE FROM att_array_devulong64_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devfloat_ro' THEN
				DELETE FROM att_scalar_devfloat_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devfloat_rw' THEN
				DELETE FROM att_scalar_devfloat_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devfloat_ro' THEN
				DELETE FROM att_array_devfloat_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devfloat_rw' THEN
				DELETE FROM att_array_devfloat_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devdouble_ro' THEN
				DELETE FROM att_scalar_devdouble_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devdouble_rw' THEN
				DELETE FROM att_scalar_devdouble_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devdouble_ro' THEN
				DELETE FROM att_array_devdouble_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devdouble_rw' THEN
				DELETE FROM att_array_devdouble_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstring_ro' THEN
				DELETE FROM att_scalar_devstring_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstring_rw' THEN
				DELETE FROM att_scalar_devstring_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devstring_ro' THEN
				DELETE FROM att_array_devstring_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devstring_rw' THEN
				DELETE FROM att_array_devstring_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstate_ro' THEN
				DELETE FROM att_scalar_devstate_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstate_rw' THEN
				DELETE FROM att_scalar_devstate_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devstate_ro' THEN
				DELETE FROM att_array_devstate_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devstate_rw' THEN
				DELETE FROM att_array_devstate_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devencoded_ro' THEN
				DELETE FROM att_scalar_devencoded_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devencoded_rw' THEN
				DELETE FROM att_scalar_devencoded_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devencoded_ro' THEN
				DELETE FROM att_array_devencoded_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devencoded_rw' THEN
				DELETE FROM att_array_devencoded_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devenum_ro' THEN
				DELETE FROM att_scalar_devenum_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devenum_rw' THEN
				DELETE FROM att_scalar_devenum_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devenum_ro' THEN
				DELETE FROM att_array_devenum_ro WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			ELSEIF table_name = 'att_array_devenum_rw' THEN
				DELETE FROM att_array_devenum_rw WHERE att_conf_id = att_id AND TIMESTAMPDIFF(MINUTE, delete_date, data_time)<0 LIMIT 10000;
			END IF;

/*
			IF table_name = 'att_scalar_devboolean_ro' THEN
				DELETE FROM att_scalar_devboolean_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devboolean_rw' THEN
				DELETE FROM att_scalar_devboolean_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devboolean_ro' THEN
				DELETE FROM att_array_devboolean_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devboolean_rw' THEN
				DELETE FROM att_array_devboolean_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devuchar_ro' THEN
				DELETE FROM att_scalar_devuchar_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devuchar_rw' THEN
				DELETE FROM att_scalar_devuchar_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devuchar_ro' THEN
				DELETE FROM att_array_devuchar_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devuchar_rw' THEN
				DELETE FROM att_array_devuchar_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devshort_ro' THEN
				DELETE FROM att_scalar_devshort_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devshort_rw' THEN
				DELETE FROM att_scalar_devshort_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devshort_ro' THEN
				DELETE FROM att_array_devshort_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devshort_rw' THEN
				DELETE FROM att_array_devshort_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devushort_ro' THEN
				DELETE FROM att_scalar_devushort_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devushort_rw' THEN
				DELETE FROM att_scalar_devushort_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devushort_ro' THEN
				DELETE FROM att_array_devushort_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devushort_rw' THEN
				DELETE FROM att_array_devushort_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong_ro' THEN
				DELETE FROM att_scalar_devlong_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong_rw' THEN
				DELETE FROM att_scalar_devlong_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong_ro' THEN
				DELETE FROM att_array_devlong_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong_rw' THEN
				DELETE FROM att_array_devlong_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong_ro' THEN
				DELETE FROM att_scalar_devulong_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong_rw' THEN
				DELETE FROM att_scalar_devulong_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong_ro' THEN
				DELETE FROM att_array_devulong_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong_rw' THEN
				DELETE FROM att_array_devulong_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong64_ro' THEN
				DELETE FROM att_scalar_devlong64_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devlong64_rw' THEN
				DELETE FROM att_scalar_devlong64_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong64_ro' THEN
				DELETE FROM att_array_devlong64_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devlong64_rw' THEN
				DELETE FROM att_array_devlong64_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong64_ro' THEN
				DELETE FROM att_scalar_devulong64_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devulong64_rw' THEN
				DELETE FROM att_scalar_devulong64_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong64_ro' THEN
				DELETE FROM att_array_devulong64_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devulong64_rw' THEN
				DELETE FROM att_array_devulong64_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devfloat_ro' THEN
				DELETE FROM att_scalar_devfloat_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devfloat_rw' THEN
				DELETE FROM att_scalar_devfloat_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devfloat_ro' THEN
				DELETE FROM att_array_devfloat_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devfloat_rw' THEN
				DELETE FROM att_array_devfloat_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devdouble_ro' THEN
				DELETE FROM att_scalar_devdouble_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devdouble_rw' THEN
				DELETE FROM att_scalar_devdouble_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devdouble_ro' THEN
				DELETE FROM att_array_devdouble_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devdouble_rw' THEN
				DELETE FROM att_array_devdouble_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstring_ro' THEN
				DELETE FROM att_scalar_devstring_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstring_rw' THEN
				DELETE FROM att_scalar_devstring_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devstring_ro' THEN
				DELETE FROM att_array_devstring_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devstring_rw' THEN
				DELETE FROM att_array_devstring_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstate_ro' THEN
				DELETE FROM att_scalar_devstate_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devstate_rw' THEN
				DELETE FROM att_scalar_devstate_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devstate_ro' THEN
				DELETE FROM att_array_devstate_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devstate_rw' THEN
				DELETE FROM att_array_devstate_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devencoded_ro' THEN
				DELETE FROM att_scalar_devencoded_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devencoded_rw' THEN
				DELETE FROM att_scalar_devencoded_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devencoded_ro' THEN
				DELETE FROM att_array_devencoded_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devencoded_rw' THEN
				DELETE FROM att_array_devencoded_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devenum_ro' THEN
				DELETE FROM att_scalar_devenum_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_scalar_devenum_rw' THEN
				DELETE FROM att_scalar_devenum_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devenum_ro' THEN
				DELETE FROM att_array_devenum_ro WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			ELSEIF table_name = 'att_array_devenum_rw' THEN
				DELETE FROM att_array_devenum_rw WHERE att_conf_id = att_id AND data_time < delete_date ORDER BY data_time LIMIT 10000;
			END IF;
*/

			-- Count number of rows to be deleted
			SET countRow = ROW_COUNT();
			SELECT countRow;
			IF countRow > 0 THEN
				SELECT CONCAT(' ',table_name,'att_conf_id=',att_id,'Deleted',countRow,'+',totCountRow,'rows up to:',delete_date);
				DO SLEEP(1);
			END IF;

			IF countRow >= 1000 THEN
				DO SLEEP(3);
			ELSEIF countRow >= 5000 THEN
				DO SLEEP(7);
			ELSEIF countRow >= 10000 THEN
				DO SLEEP(14);
			END IF;

			SET totCountRow = totCountRow + countRow;
			DO SLEEP(1);
		UNTIL countRow <= 0 
		
		END REPEAT;
		
		DO SLEEP(1);
	END WHILE;


END #

/* Finally, reset the delimiter to the default ; */
DELIMITER ;

DELIMITER #


/* =========================================== */
-- ===        CREATE MAIN PROCEDURE
/* =========================================== */
drop procedure if exists delete_archive_old_data;
CREATE PROCEDURE delete_archive_old_data (IN dt INT)
proc_main: BEGIN

	DECLARE attr_name VARCHAR(1024);
	DECLARE delete_date DATETIME;
	DECLARE bDone INT;
	DECLARE foundData INT;
	DECLARE curs CURSOR FOR SELECT att_name FROM att_conf;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET bDone = TRUE;
	
	-- Set delete date	
	SET delete_date= DATE_SUB(CURRENT_DATE(), INTERVAL dt MINUTE);
	SELECT dt;
	SELECT delete_date;
	
	-- Loop over attributes and delete old data
	OPEN curs;
		SET bDone = FALSE;
		read_loop: LOOP
			FETCH curs INTO attr_name;
			IF bDone THEN
      	LEAVE read_loop;
      END IF;
			SELECT CONCAT('INFO: Deleting attr ',attr_name,' archived data older than ',delete_date,' ...');
		
			CALL delete_old_attr(attr_name,delete_date);
			DO SLEEP(1);
		END LOOP;
	CLOSE curs;

/*
	SET attr_name= 'tango://riggi-tpt460:10000/mid_dish_0001/spf/emulator/b1LnaVDrainCurrent';
	SELECT CONCAT('INFO: Deleting attr ',attr_name,' archived data older than ',delete_date,' ...');
	CALL delete_old_attr(attr_name,delete_date);
*/

END proc_main #

/* Finally, reset the delimiter to the default ; */
DELIMITER ;

/* =========================================== */
-- ===        EXECUTE MAIN PROCEDURE
/* =========================================== */
CALL delete_archive_old_data(@time_interval);


