/*
 *  Utils functions for Api communications 
 *
 */




// export async function fetch_wrapper(route, method_type, body_stringified, handler)
// {
//     const response = await fetch('/auth/login', {
//                           method: method_type,
//                           headers: { 'Content-Type': 'application/json' },
//                           body: body_stringified,
//                           headers: { 'Authorization':  "Bearer " + token},
//                             })

//     let res = await response.json();
//     let status = response.status;

//     handler(status, res, success_callback, set_error);

// }