<?php

global $global, $conf;

define(ANONYMOUS_USER, 0);

include_once APPROOT . '/inc/security/lib_auth.inc';
include_once APPROOT . '/inc/security/lib_acl_cas.inc';

if (isset($_POST['login']) && 'login' == $_POST['login']) {
    $captchavalid = true;
    if ($conf['use_recaptcha']) {
        require_once(APPROOT . '3rd/recaptcha/recaptchalib.php');
        $publickey = $conf['recaptcha_public_key'];
        $privatekey = $conf['recaptcha_private_key'];
        $resp = recaptcha_check_answer($privatekey, $_SERVER["REMOTE_ADDR"], $_POST["recaptcha_challenge_field"], $_POST["recaptcha_response_field"]);

        if (!$resp->is_valid) {
            $global['loginerror'] = _t("The reCAPTCHA wasn't entered correctly.");
            $captchavalid = false;
        }
    }

    $username = addslashes($_POST['username']);
    $password = addslashes($_POST['password']);

    $user = UserHelper::loadFromUsername($username);
    if ($captchavalid) {
        if ($user == null) {
            Log::saveLogDetails('login', $_SERVER['REMOTE_ADDR'], "NOTOK", "NOTOK",null,'login',$username);
            $global['nouser'] = _t('USER_NOT_FOUND__INVALID_USERNAME_OR_PASSWORD__PLEASE_TRY_AGAIN_');
        } else if ($user->status != 'active') {
            Log::saveLogDetails('login', $_SERVER['REMOTE_ADDR'], "NOTOK", "NOTOK",null,'login',$username);
            $global['loginerror'] = _t('INVALID_USERNAME_OR_PASSWORD__PLEASE_TRY_AGAIN_');
        } else {
            if ($user->isPasswordMatch($password)) {
                Log::saveLogDetails('login', $_SERVER['REMOTE_ADDR'], "OK", "OK",null,'login',$username);
                //if so registor the user in the session.
                //MUST REMEMBER ALLWAYS REGENERATE SESSION ID WHEN PERMISSION LEVEL CHANGES *******
                if (!isset($_COOKIE['cookie-check'])) {
                    // cookies disabled
                    $global['loginerror'] = _t("YOUR_BROWSER_S_COOKIE_FUNCTIONALITY_IS_TURNED_OFF__PLEASE_TURN_IT_ON");
                } else {
                    session_regenerate_id();
                    $_SESSION['username'] = $user->getUserName();
                    $cfg = array();
                    if (!empty($user->config)) {
                        $cfg = @json_decode($user->config, true);
                    }
                    if ($cfg['locale']) {
                       $conf['locale'] = $cfg['locale'];
                    }
    
                    if(isset($cfg['security']['TSV']['method'])) {
                        $_SESSION['check_TSV'] = $cfg['security']['TSV']['method'];
                    }
                }
            }
            else{
                Log::saveLogDetails('login', $_SERVER['REMOTE_ADDR'], "NOTOK", "NOTOK",null,'login',$username);
                $global['loginerror'] = _t('INVALID_USER_ID_OR_PASSWORD__PLEASE_TRY_AGAIN_');
            }
        }
    }
} elseif( !empty($_SESSION['check_TSV']) && !empty($_POST['code']) ) {
    if($_SESSION['check_TSV'] == "MGA")
        $TSVResult = checkMGAAuth($global['loginerror']);
    
    if($_SESSION['check_TSV'] == "yubikey")
        $TSVResult = checkYubikeyAuth($global['loginerror']);
} else {
    setcookie("cookie-check", "some-value");
}

function checkMGAAuth(&$error) {
    $user = UserHelper::loadFromUsername($_SESSION['username']);
    if($user->verifyGACode($_POST['code'])){
        unset($_SESSION['check_TSV']);
        return true;
    }

    $error = _t('Wrong verification code. Please try again.');
}

function checkYubikeyAuth(&$error) {
    global $conf;

    require_once APPROOT . "3rd/yubico/Yubico.php";
    
    $yubi = new Auth_Yubico($conf['YubiKeyClientId'], $conf['YubiKeyClientKey'], 1, 1);

    $parsedCode = $yubi->parsePasswordOTP($_POST['code']);
    
    if(!$parsedCode) {
        $error = _t("Could not parse a valid yubikey code");
        return false;
    }    

    $userConfig = UserHelper::loadFromUsername($_SESSION['username'])->getConfig();
    $keyIdMatches = ($parsedCode['prefix'] == $userConfig['security']['TSV']['keyId']);

    if(!$keyIdMatches) {
        $error = _t("OTP does not match user's paired YubiKey.");
        return false;
    }

    $auth = $yubi->verify($_POST['code']);

    if (PEAR::isError($auth)) {
        $error = _t($auth->getMessage());
        return false;
    } else {
        unset($_SESSION['check_TSV']);
        return true;
    }
}

//check if a remember key is set
/* if(!isset($_SESSION['username']) && isset($_COOKIE["R"]) && isset($_COOKIE["U"])){
  $code = shn_auth_getcode( $_COOKIE["U"] ,'remember');
  $value = shn_auth_generateHash($_SERVER['HTTP_USER_AGENT'] , $code);
  if($_COOKIE["R"]==$value)
  $_SESSION['username'] = $_COOKIE["U"];
  }
 */

//check if the action is log out
if ($_GET['act'] == 'logout') {
    //remove any remember key if exists     
    shn_auth_del_code($_SESSION['username'], 'remember');

    session_regenerate_id();
    session_unset();
    session_destroy();
    $_SESSION['username'] = null;
    if (isCasAuth()) { // cas auth
        casLogout();
    }
}

if($_GET['mod'] != 'home' || $_GET['act'] !='download' ){
    //check if the user session exists
    if (!isset($_SESSION['username']) || $_SESSION['check_TSV']) {
        //normal auth
        include_once APPROOT . '/tpls/html_login.php';
        exit(0);

        //if not set the user as anonymous
        //$_SESSION['username']=ANONYMOUS_USER;
        //$global['username']=ANONYMOUS_USER;
    } else {
        //if exists set the global username 
        $global['username'] = $_SESSION['username'];
    }
}
?>
