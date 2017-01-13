

SHDB=$(top_builddir)/src/share-util/static/shdb
SHRM=$(top_builddir)/src/share-util/static/shrm
JSON_ZIPCODE=$(top_srcdir)/db/libshare/geo/zipcode.json
JSON_COMMON=$(top_srcdir)/db/libshare/geo/common.json
JSON_CITY=$(top_srcdir)/db/libshare/geo/city.json
JSON_NETWORK=$(top_srcdir)/db/libshare/geo/network.json

install-exec-hook: $(JSON_ZIPCODE) $(JSON_COMMON)
	@echo "Installing geodetic databases.."
	$(SHRM) "/sys/db/libshare/geo"
	$(SHDB) geo --import $(JSON_ZIPCODE)
	$(SHDB) geo --import $(JSON_CITY)
	$(SHDB) geo --import $(JSON_COMMON)
	$(SHDB) geo --import $(JSON_NETWORK)

