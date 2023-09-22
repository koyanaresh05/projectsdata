// This function is the endpoint's request handler.
//exports = function({ query, headers, body}, response) {
exports = function(payload){  
    // Data can be extracted from the request as follows:

    // Query params, e.g. '?arg1=hello&arg2=world' => {arg1: "hello", arg2: "world"}
   // const {arg1, arg2} = query;

    // Headers, e.g. {"Content-Type": ["application/json"]}
   // const contentTypes = headers["Content-Type"];

    // Raw request body (if the client sent one).
    // This is a binary object that can be accessed as a string using .text()
  //  const reqBody = body;

   // console.log("arg1, arg2: ", arg1, arg2);
  //  console.log("Content-Type:", JSON.stringify(contentTypes));
  //  console.log("Request body:", reqBody);

    // You can use 'context' to interact with other Realm features.
    // Accessing a value:
    // var x = context.values.get("value_name");

    // Querying a mongodb service:
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").findOne();
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find();
  
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"Safe"});
  
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"UnSafe"});  
    const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"name":payload.body.text()});

    // Calling a function:
    // const result = context.functions.execute("function_name", arg1, arg2);

    // The return value of the function is sent as the response back to the client
    // when the "Respond with Result" setting is set.
   // return  "Hello World!";
   return  doc;
};




/*
exports = function(payload){
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("ASSET_INFO").collection("New_Luggage_data");
    try {
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        body['ts'] = new Date();
      }
      coll.insertOne(body);
      console.log(body);

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
};
*/