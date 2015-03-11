#curl socu.creal.de/api/v1/datastreams/heartrate -X DELETE
#curl socu.creal.de/api/v1/datastreams/heartrate -H "Content-Type: application/json" -d '{ "data_fetch_method": "GET", "what_to_submit": null, "update_interval": 100, "default_value": 0, "value": 80, "nominal_range": [ 0, 180 ], "nominal_description": null, "recommended_nominal_mapping_range": [ 0, 10 ], "recommended_stimulations": [ "vibration" ], "name": "heartrate", "description": "Live Heart Rate dataset.", "created_at": 1424776903294, "last_updated": 1425409788392, "links": [ { "rel": "self", "href": "http://socu.creal.de/api/v1/datastreams/heartrate" } ] }'


while true; do
  interval=$((40+($RANDOM%100)))
  curl socu.creal.de/api/v1/datastreams/heartrate -X PUT -H "Content-Type:application/json" -d "{ \"value\": $interval }"
  sleep $(($RANDOM%10))
done
