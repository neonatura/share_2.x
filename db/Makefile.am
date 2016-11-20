

SHDB=$(top_builddir)/src/share-util/static/shdb
JSON_ZIPCODE=$(top_srcdir)/db/libshare/geo/zipcode.json
JSON_COMMON=$(top_srcdir)/db/libshare/geo/common.json
JSON_CITY=$(top_srcdir)/db/libshare/geo/city.json
JSON_NETWORK=$(top_srcdir)/db/libshare/geo/network.json

install-exec-hook: $(JSON_ZIPCODE) $(JSON_COMMON)
	@echo "Installing geodetic database (city).."
	$(SHDB) geo "drop table sys_city_NA;" -i
	$(SHDB) geo --import $(JSON_CITY)
	@echo "Installing geodetic database (zipcode).."
	$(SHDB) geo "drop table sys_zipcode_NA;" -i
	$(SHDB) geo --import $(JSON_ZIPCODE)
	@echo "Installing geodetic database (common).."
	$(SHDB) geo "drop table sys_common_NA;" -i
	$(SHDB) geo --import $(JSON_COMMON)
	@echo "Installing geodetic database (network).."
	$(SHDB) geo "drop table sys_network_NA;" -i
	$(SHDB) geo --import $(JSON_NETWORK)
