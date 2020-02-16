/*
 *  Utils functions for Login 
 *
 */


export async function fetch_login(credentials, handleLogin, set_error)
{
    const response = await fetch('/auth/login', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json' },
                          body: JSON.stringify(credentials),
                            })

    try
    {
      let res = await response.json();
      let status = response.status;
      if (status === 200)
      {
          handleLogin(res, credentials);
      }
      else
      {
          set_error(true)
         console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
      }
    }
    catch (err)
    {
        set_error(true)
        console.error(`Request rejected with status ${response.status}  and message ${JSON.stringify(response)}`)
    }
}

